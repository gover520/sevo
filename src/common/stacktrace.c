/*
 *  stacktrace.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include <mclib.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stacktrace.h"

#ifdef _WIN32
# include <Windows.h>
# include <DbgHelp.h>
# include <signal.h>

typedef BOOL (WINAPI *pfn_SymInitialize)(HANDLE, LPSTR, BOOL);
typedef BOOL (WINAPI *pfn_SymCleanup)(HANDLE);
typedef DWORD (WINAPI *pfn_SymGetOptions)(VOID);
typedef DWORD (WINAPI *pfn_SymSetOptions)(DWORD);
typedef BOOL (WINAPI *pfn_StackWalk64)(DWORD, HANDLE, HANDLE,
    LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, 
    PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);
typedef BOOL (WINAPI *pfn_SymGetSymFromAddr64)(HANDLE, DWORD64, PDWORD64, PIMAGEHLP_SYMBOL64);
typedef BOOL (WINAPI *pfn_SymGetLineFromAddr64)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
#else
# include <execinfo.h>
# include <unistd.h>
#endif

static char g_outfile[MC_MAX_PATH] = { 0 };

#ifdef _WIN32
static void stackwalker(FILE *fp, CONTEXT *context) {
    STACKFRAME64 s = { 0 };
    CONTEXT c = { 0 };
    DWORD imageType, symOptions, dis = 0;
    DWORD64 dis64 = 0;
    INT frameCount, count = 0;
    CHAR symbolName[MAX_SYM_NAME + 1];
    CHAR buffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_SYM_NAME + 1] = { 0 };
    HANDLE hProcess, hThread;
    IMAGEHLP_LINE64 line;
    IMAGEHLP_SYMBOL64 *pSym = NULL;

    mc_dylib_t mod_dbghelp = NULL;
    pfn_SymInitialize pfSymInitialize = NULL;
    pfn_SymCleanup pfSymCleanup = NULL;
    pfn_SymGetOptions pfSymGetOptions = NULL;
    pfn_SymSetOptions pfSymSetOptions = NULL;
    pfn_StackWalk64 pfStackWalk64 = NULL;
    pfn_SymGetSymFromAddr64 pfSymGetSymFromAddr64 = NULL;
    pfn_SymGetLineFromAddr64 pfSymGetLineFromAddr64 = NULL;

    hProcess = GetCurrentProcess();
    hThread = GetCurrentThread();

    if (!mod_dbghelp) {
        mc_dir_name(symbolName, mc_self_name(), MC_PATHSEP);
        strcat(symbolName, "\\dbghelp.dll");

        mod_dbghelp = mc_dl_open(symbolName);
    }

    if (!mod_dbghelp) {
        mc_getcwd(symbolName, sizeof(symbolName));
        strcat(symbolName, "\\dbghelp.dll");

        mod_dbghelp = mc_dl_open(symbolName);
    }

    if (!mod_dbghelp) {
        mod_dbghelp = mc_dl_open("dbghelp.dll");
    }

    if (!mod_dbghelp) {
        fprintf(fp, "Error while loading dbghelp.dll\n");
        return;
    }

    pfSymInitialize = (pfn_SymInitialize)mc_dl_symbol(mod_dbghelp, "SymInitialize");
    pfSymGetOptions = (pfn_SymGetOptions)mc_dl_symbol(mod_dbghelp, "SymGetOptions");
    pfSymSetOptions = (pfn_SymSetOptions)mc_dl_symbol(mod_dbghelp, "SymSetOptions");
    pfStackWalk64 = (pfn_StackWalk64)mc_dl_symbol(mod_dbghelp, "StackWalk64");
    pfSymGetSymFromAddr64 = (pfn_SymGetSymFromAddr64)mc_dl_symbol(mod_dbghelp, "SymGetSymFromAddr64");
    pfSymGetLineFromAddr64 = (pfn_SymGetLineFromAddr64)mc_dl_symbol(mod_dbghelp, "SymGetLineFromAddr64");
    pfSymCleanup = (pfn_SymCleanup)mc_dl_symbol(mod_dbghelp, "SymCleanup");

    if (!pfSymInitialize || !pfSymGetOptions || !pfSymSetOptions || !pfStackWalk64 
        || !pfSymGetSymFromAddr64 || !pfSymGetLineFromAddr64 || !pfSymCleanup) {
        fprintf(fp, "Error while initializing dbghelp.dll\n");
        return;
    }

    symOptions = pfSymGetOptions();

    symOptions |= SYMOPT_LOAD_LINES;
    symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;

    pfSymSetOptions(symOptions);

    if (!pfSymInitialize(hProcess, NULL, TRUE)) {
        fprintf(fp, "SymInitialize GetLastError: %u\n", GetLastError());
        return;
    }

    if (context) {
        c = *context;
    } else {
        c.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&c);
    }

#ifdef _M_IX86
    imageType = IMAGE_FILE_MACHINE_I386;
    s.AddrPC.Offset = c.Eip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = c.Ebp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.Esp;
    s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64 || _M_AMD64
    imageType = IMAGE_FILE_MACHINE_AMD64;
    s.AddrPC.Offset = c.Rip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = c.Rbp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.Rsp;
    s.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
    imageType = IMAGE_FILE_MACHINE_IA64;
    s.AddrPC.Offset = c.StIIP;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = c.IntSp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrBStore.Offset = c.RsBSP;
    s.AddrBStore.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.IntSp;
    s.AddrStack.Mode = AddrModeFlat;
#else
# error "Platform not supported!"
#endif

    pSym = (IMAGEHLP_SYMBOL64 *)buffer;
    pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    pSym->MaxNameLength = MAX_SYM_NAME;

    memset(&line, 0, sizeof(line));
    line.SizeOfStruct = sizeof(line);

    for (frameCount = 0; ; ++frameCount) {
        if (!pfStackWalk64(imageType, hProcess, hThread, &s, &c, NULL, NULL, NULL, NULL)) {
            fprintf(fp, "Error: StackWalk64, GetLastError: %u (Address: %p)\n", GetLastError(), (LPVOID)s.AddrPC.Offset);
            break;
        }

        if (!pfSymGetSymFromAddr64(hProcess, s.AddrPC.Offset, &dis64, pSym)) {
            fprintf(fp, "Error: SymGetSymFromAddr64, GetLastError: %u (Address: %p)\n", GetLastError(), (LPVOID)s.AddrPC.Offset);
            continue;
        }
        strcpy(symbolName, pSym->Name);

        if (!pfSymGetLineFromAddr64(hProcess, s.AddrPC.Offset, &dis, &line)) {
            fprintf(fp, "%s Error: SymGetLineFromAddr64, GetLastError: %u (Address: %p)\n", symbolName, GetLastError(), (LPVOID)s.AddrPC.Offset);
            continue;
        }
        fprintf(fp, "%d -> %s (%s, %d)\n", ++count, symbolName, line.FileName, line.LineNumber);

        if (0 == s.AddrReturn.Offset) {
            break;
        }
    }

    pfSymCleanup(hProcess);
    mc_dl_close(mod_dbghelp);
}

static void output_stacktrace(FILE *fp) {
    stackwalker(fp, NULL);
}

static LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS * ExceptionInfo) {
    FILE *fp;

    if (*g_outfile) {
        fp = fopen(g_outfile, "w");
        if (fp) {
            stackwalker(fp, ExceptionInfo->ContextRecord);
            fclose(fp);
        }
    } else {
        stackwalker(stderr, ExceptionInfo->ContextRecord);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}
#else
static void output_stacktrace(FILE *fp) {
    void *arr[1024] = { NULL };
    size_t size, i;
    char **strings;

    size = backtrace(arr, MC_ARRAY_SIZE(arr));
    strings = backtrace_symbols(arr, size);

    if (strings) {
        for (i = 0; i < size; ++i) {
            fprintf(fp, "%d -> %s\n", (int)(size - i), strings[i]);
        }
        free(strings);
    }
}
#endif

static void signal_handler(int sig) {
    FILE *fp;

    if (*g_outfile) {
        fp = fopen(g_outfile, "w");
        if (fp) {
            output_stacktrace(fp);
            fclose(fp);
        }
    } else {
        output_stacktrace(stderr);
    }

    exit(EXIT_SUCCESS);
}

int install_stacktrace(const char *outfile) {
#ifdef _WIN32
    SetUnhandledExceptionFilter(windows_exception_handler);

    signal(SIGTERM, signal_handler);
#endif

    signal(SIGFPE, signal_handler);     /* divide by 0 */
    signal(SIGSEGV, signal_handler);    /* segmentation fault */
    signal(SIGABRT, signal_handler);

    if (outfile) {
        strncpy(g_outfile, outfile, sizeof(g_outfile));
    }

    return 0;
}
