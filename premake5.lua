-- A solution contains projects, and defines the available configurations
solution ( "sevo" )
    configurations { "Release", "Debug" }
    platforms { "x64" }

    if _ACTION == "clean" then
        os.rmdir(".vs")
        os.rmdir("bin")
        os.rmdir("objs")
        os.remove("sevo.VC.db")
        os.remove("sevo.sln")
        os.remove("sevo.vcxproj")
        os.remove("sevo.vcxproj.filters")
        os.remove("sevo.vcxproj.user")
        os.remove("sevo.make")
        os.remove("Makefile")
        return
    end

    local function shell_exec(cmd)
        local h = io.popen(cmd)
        local d = h:read()
        h:close()
        return d
    end

    local function get_dirname(p)
        return path.getdirectory(p:gsub("\\", "/"))
    end

    lua_incdir = os.getenv("LUA_INCLUDE_DIR")
    lua_libdir = os.getenv("LUA_LIBRARIES")
    lua_bindir = os.getenv("LUA_BINARIES")
    lua_lib = "lua"
    lua_dll = "lua.dll"

    if lua_bindir == nil then
        if os.target() == "windows" then
            local e = shell_exec("where lua")
            if e ~= nil then
                lua_bindir = get_dirname(e)
            end

            if lua_bindir == nil then
                e = shell_exec("where lua53")
                if e ~= nil then
                    lua_bindir = get_dirname(e)
                end
            end

            if lua_bindir == nil then
                e = shell_exec("where lua5.3")
                if e ~= nil then
                    lua_bindir = get_dirname(e)
                end
            end
        else
            local e = shell_exec("which lua")
            if e ~= nil then
                lua_bindir = get_dirname(e)
            end
        end
    end
    print("Lua bindir: " .. lua_bindir)

    if lua_incdir == nil then
        lua_incdir = os.findheader("lua.h")

        if lua_incdir == nil and os.target() == "windows" then
            lua_incdir = path.join(lua_bindir, "include")
        end
    end
    print("Lua incdir: " .. lua_incdir)

    if lua_libdir == nil then
        lua_libdir = os.findlib("lua")

        if lua_libdir == nil then
            lua_libdir = os.findlib("liblua.a")
        end

        if lua_libdir == nil and os.target() == "windows" then
            local l = os.matchfiles(path.join(lua_bindir, "lua*.lib"))
            if l ~= nil then
                lua_libdir = lua_bindir
                lua_lib = path.getname(l[1])
                print("Lua lib: " .. lua_lib)
            end

            local d = os.matchfiles(path.join(lua_bindir, "lua*.dll"))
            if d ~= nil then
                lua_dll = path.getname(d[1])
                print("Lua dll: " .. lua_dll)
            end
        end
    end
    print("Lua libdir: " .. lua_libdir)

    -- A project defines one build target
    project ( "sevo" )
        kind ( "ConsoleApp" )
        language ( "C" )
        targetname ("sevo")
        includedirs { "./src", lua_incdir,
                        "./src/libraries/mclib",
                        "./src/libraries/mini-gmp",
                        "./src/libraries/luaffifb",
                        "./src/libraries/luaffifb/dynasm",
                        "./src/libraries/lpeg",
                        "./src/libraries/physfs/src",
                        "./src/libraries/tlsf",
                        "./src/libraries/liblmdb", }
        files { "./src/*.h", "./src/*.c",
                "./src/common/**.*", "./src/modules/**.*",
                "./src/libraries/mini-gmp/*.h", "./src/libraries/mini-gmp/*.c",
                "./src/libraries/luaffifb/*.h", "./src/libraries/luaffifb/*.c",
                "./src/libraries/lpeg/*.h", "./src/libraries/lpeg/*.c",
                "./src/libraries/physfs/src/*.h", "./src/libraries/physfs/src/*.c",
                "./src/libraries/tlsf/*.h", "./src/libraries/tlsf/*.c",
                "./src/libraries/liblmdb/lmdb.h", "./src/libraries/liblmdb/mdb.c",
                "./src/libraries/liblmdb/midl.h", "./src/libraries/liblmdb/midl.c", }
        excludes { "./src/libraries/luaffifb/test.c", }
        defines { "_UNICODE",
                    "PHYSFS_SUPPORTS_GRP=0", "PHYSFS_SUPPORTS_HOG=0",
                    "PHYSFS_SUPPORTS_MVL=0", "PHYSFS_SUPPORTS_WAD=0",
                    "PHYSFS_SUPPORTS_QPAK=0", "PHYSFS_SUPPORTS_SLB=0",
                    "PHYSFS_SUPPORTS_ISO9660=0", "PHYSFS_SUPPORTS_VDF=0",
                    "PHYSFS_SUPPORTS_7Z=0", "PHYSFS_NO_CDROM_SUPPORT=1", }
        flags { "StaticRuntime" }
        libdirs { lua_libdir }
        links { lua_lib }

        configuration ( "Release" )
            optimize "On"
            objdir ( "./objs" )
            targetdir ( "./bin" )
            defines { "NDEBUG", "_NDEBUG" }

        configuration ( "Debug" )
            symbols "On"
            objdir ( "./objs" )
            targetdir ( "./bin" )
            defines { "DEBUG", "_DEBUG" }

        configuration ( "vs*" )
            defines { "WIN32", "_WIN32", "_WINDOWS",
                        "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE",
                        "_CRT_NONSTDC_NO_DEPRECATE", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
            postbuildcommands {
                "xcopy /Y " .. path.join(lua_bindir, lua_dll):gsub("/", "\\") .. " " .. path.join(".", "bin"):gsub("/", "\\")
            }

        configuration ( "gmake" )
            warnings  "Default" --"Extra"
            defines { "LINUX_OR_MACOSX" }

        configuration { "gmake", "macosx" }
            files { "./src/libraries/physfs/src/*.m" }
            defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }
            links { "Foundation.framework", "IOKit.framework" }

        configuration { "gmake", "linux" }
            defines { "__linux__" }
            links { "m", "pthread", "dl" }
