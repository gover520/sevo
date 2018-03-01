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
        os.remove("sevo.make")
        os.remove("Makefile")
        return
    end

    lua_include_dir = os.getenv("LUA_INCLUDE_DIR")
    lua_libraries = os.getenv("LUA_LIBRARIES")
    lualib = "lua"

    if lua_include_dir == nil then
        lua_include_dir = os.findheader(lualib)
    end

    print("Lua include: " .. lua_include_dir)

    if lua_libraries == nil then
        lua_libraries = os.findlib(lualib)
    end

    print("Lua libdir: " .. lua_libraries)

    sysincludedirs { lua_include_dir }
    syslibdirs { lua_libraries }

    if os.target() == "windows" then
        luadll = "lua.dll"

        local l = os.matchfiles(lua_libraries .. "/lua*.lib")
        if l ~= nil then
            lualib = path.getname(l[1])
            print("Lua lib: " .. lualib)
        end

        local d = os.matchfiles(lua_libraries .. "/lua*.dll")
        if d ~= nil then
            luadll = path.getname(d[1])
            print("Lua dll: " .. luadll)
        end
    end

    -- A project defines one build target
    project ( "sevo" )
        kind ( "ConsoleApp" )
        language ( "C" )
        targetname ("sevo")
        includedirs { "./src", lua_include_dir,
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
        libdirs { lua_libraries }
        links { lualib }

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
                "xcopy /Y " .. lua_libraries .. "\\" .. luadll .. " .\\bin\\"
            }

        configuration ( "gmake" )
            warnings  "Extra"
            defines { "LINUX_OR_MACOSX" }

        configuration { "gmake", "macosx" }
            files { "./src/libraries/physfs/src/*.m" }
            defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }
            links { "Foundation.framework", "IOKit.framework" }

        configuration { "gmake", "linux" }
            defines { "__linux__" }
            links { "m", "pthread", "dl" }
