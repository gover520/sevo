/*
 *  allocator.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "allocator.h"
#include <stdlib.h>

#define USE_TLSF            1

#if USE_TLSF
# include <tlsf.h>

# ifndef _WIN32
#  include <unistd.h>
#  include <sys/mman.h>
# else  /* _WIN32 */
#  include <Windows.h>
#  include <errno.h>
#  include <io.h>

# define PROT_NONE          0
# define PROT_READ          1
# define PROT_WRITE         2
# define PROT_EXEC          4

# define MAP_FILE           0
# define MAP_SHARED         1
# define MAP_PRIVATE        2
# define MAP_TYPE           0x0F
# define MAP_FIXED          0x10
# define MAP_ANONYMOUS      0x20
# define MAP_ANON           MAP_ANONYMOUS

# define MAP_FAILED         ((void *)-1)

# define MS_ASYNC           1
# define MS_SYNC            2
# define MS_INVALIDATE      4

# ifndef FILE_MAP_EXECUTE
#  define FILE_MAP_EXECUTE  0x0020
# endif

static int _mmap_error(DWORD err, int deferr) {
    if (0 == err) {
        return deferr;
    }
    return err;
}

static DWORD _mmap_prot_page(int prot) {
    DWORD protect = 0;

    if (PROT_NONE == prot) {
        return protect;
    }

    if (prot & PROT_EXEC) {
        protect = (prot & PROT_WRITE) ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
    } else {
        protect = (prot & PROT_WRITE) ? PAGE_READWRITE : PAGE_READONLY;
    }

    return protect;
}

static DWORD _mmap_prot_file(int prot) {
    DWORD desiredAccess = 0;

    if (PROT_NONE == prot) {
        return desiredAccess;
    }

    if (prot & PROT_READ) {
        desiredAccess |= FILE_MAP_READ;
    }
    if (prot & PROT_WRITE) {
        desiredAccess |= FILE_MAP_WRITE;
    }
    if (prot & PROT_EXEC) {
        desiredAccess |= FILE_MAP_EXECUTE;
    }

    return desiredAccess;
}

static void *mmap(void *addr, size_t len, int prot, int flags, int fildes, unsigned long off) {
    HANDLE fm, h;
    void * map = MAP_FAILED;

    DWORD protect = _mmap_prot_page(prot);
    DWORD desiredAccess = _mmap_prot_file(prot);

    DWORD dwFileOffsetHigh = 0;
    DWORD dwFileOffsetLow = (DWORD)off;

    DWORD dwMaxSizeHigh = 0;
    DWORD dwMaxSizeLow = (DWORD)(off + len);

    errno = 0;

    if (!len
        /* Unsupported flag combinations */
        || (flags & MAP_FIXED)
        /* Usupported protection combinations */
        || (PROT_EXEC == prot)) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    h = !(flags & MAP_ANONYMOUS) ? (HANDLE)_get_osfhandle(fildes) : INVALID_HANDLE_VALUE;

    if ((INVALID_HANDLE_VALUE == h) && !(flags & MAP_ANONYMOUS)) {
        errno = EBADF;
        return MAP_FAILED;
    }

    fm = CreateFileMapping(h, NULL, protect, dwMaxSizeHigh, dwMaxSizeLow, NULL);

    if (!fm) {
        errno = _mmap_error(GetLastError(), EPERM);
        return MAP_FAILED;
    }

    map = MapViewOfFile(fm, desiredAccess, dwFileOffsetHigh, dwFileOffsetLow, len);

    CloseHandle(fm);

    if (!map) {
        errno = _mmap_error(GetLastError(), EPERM);
        return MAP_FAILED;
    }

    return map;
}

static int munmap(void *addr, size_t len) {
    if (!UnmapViewOfFile(addr)) {
        errno = _mmap_error(GetLastError(), EPERM);
        return -1;
    }
    return 0;
}

static int msync(void *addr, size_t len, int flags) {
    if (!FlushViewOfFile(addr, len)) {
        errno = _mmap_error(GetLastError(), EPERM);
        return -1;
    }
    return 0;
}

static int mprotect(void *addr, size_t len, int prot) {
    DWORD newProtect = _mmap_prot_page(prot);
    DWORD oldProtect = 0;

    if (!VirtualProtect(addr, len, newProtect, &oldProtect)) {
        errno = _mmap_error(GetLastError(), EPERM);
        return -1;
    }
    return 0;
}

static int mlock(const void *addr, size_t len) {
    if (!VirtualLock((LPVOID)addr, len)) {
        errno = _mmap_error(GetLastError(), EPERM);
        return -1;
    }
    return 0;
}

static int munlock(const void *addr, size_t len) {
    if (!VirtualUnlock((LPVOID)addr, len)) {
        errno = _mmap_error(GetLastError(), EPERM);
        return -1;
    }
    return 0;
}
# endif /* _WIN32 */

# ifndef MAP_ANONYMOUS
#  define MAP_ANONYMOUS      MAP_ANON
# endif

# define DEFAULT_AREA_SIZE   16384

static void *get_new_area(size_t *size) {
    static size_t pagesize = 0;
    void *area;

    if (!pagesize) {
# ifndef _WIN32
        pagesize = sysconf(_SC_PAGESIZE);
# else
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        pagesize = info.dwPageSize;
# endif
    }

    *size = MC_ROUNDUP(*size, pagesize);
    area = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    return (MAP_FAILED != area) ? area : NULL;
}

typedef struct tlsf_ctx_t {
    tlsf_t      tlsf;
    int         spinlock;
} tlsf_ctx_t;

static tlsf_ctx_t   tlsf_ctx = { 0 };

static MC_DECLARE_ALLOC_CB(tlsf_allocator, ptr, size, file, func, line) {
    void *mem = NULL;

    mc_spin_lock(&tlsf_ctx.spinlock);

    if (!tlsf_ctx.tlsf) {
        size_t area_size;
        void *area;

        area_size = tlsf_block_size_min() + size;
        area_size = (area_size > DEFAULT_AREA_SIZE) ? area_size : DEFAULT_AREA_SIZE;

        area = get_new_area(&area_size);

        if (area) {
            tlsf_ctx.tlsf = tlsf_create_with_pool(area, area_size);
        }
    }

    if (tlsf_ctx.tlsf) {
        mem = tlsf_realloc(tlsf_ctx.tlsf, ptr, size);

        if (!mem && size) {
            size_t area_size;
            void *area;

            area_size = tlsf_block_size_min() + size;
            area_size = (area_size > DEFAULT_AREA_SIZE) ? area_size : DEFAULT_AREA_SIZE;

            area = get_new_area(&area_size);

            if (area) {
                pool_t pool = tlsf_add_pool(tlsf_ctx.tlsf, area, area_size);
                /* todo: pool should be managed. */
                mem = tlsf_realloc(tlsf_ctx.tlsf, ptr, size);
            }
        }
    }

    mc_spin_unlock(&tlsf_ctx.spinlock);

    return mem;
}
#else   /* USE_TLSF */
static MC_DECLARE_ALLOC_CB(default_allocator, ptr, size, file, func, line) {
    if (size) {
        return realloc(ptr, size);
    }
    free(ptr);
    return NULL;
}
#endif  /* USE_TLSF */

MC_ALLOC_CB_TYPE get_allocator() {
#if USE_TLSF
    return tlsf_allocator;
#else
    return default_allocator;
#endif
}

void allocator_cleanup(void) {
#if USE_TLSF
    /* todo: free tlsf and pool. */
#endif
}
