-- A solution contains projects, and defines the available configurations
solution ( "sevo" )
    configurations { "Release", "Debug" }
    platforms { "x64" }

    if _ACTION == "clean" then
        os.rmdir(".vs")
        os.rmdir("bin")
        os.rmdir("objs")
        os.remove("sevo.VC.db")
    end

    -- A project defines one build target
    project ( "sevo" )
        kind ( "ConsoleApp" )
        language ( "C" )
        targetname ("sevo")
        includedirs { "./src", "./src/libraries/mclib",
                        "./src/libraries/lua/src",
                        "./src/libraries/mini-gmp",
                        "./src/libraries/luaffifb",
                        "./src/libraries/luaffifb/dynasm",
                        "./src/libraries/lpeg",
                        "./src/libraries/physfs/src", }
        --libdirs { "" }
        --links { "" }
        files { "./src/*.h", "./src/*.c",
                "./src/common/**.*", "./src/modules/**.*",
                "./src/libraries/lua/src/*.h",
                "./src/libraries/lua/src/*.c",
                "./src/libraries/mini-gmp/*.h",
                "./src/libraries/mini-gmp/*.c",
                "./src/libraries/luaffifb/*.h",
                "./src/libraries/luaffifb/*.c",
                "./src/libraries/lpeg/*.h",
                "./src/libraries/lpeg/*.c",
                "./src/libraries/luasocket/src/*.h",
                "./src/libraries/luasocket/src/*.c",
                "./src/libraries/physfs/src/*.h",
                "./src/libraries/physfs/src/*.c", }
        excludes { "./src/libraries/lua/src/lua.c",
                    "./src/libraries/lua/src/luac.c",
                    "./src/libraries/luaffifb/test.c",
                    "./src/libraries/luasocket/src/serial.c" }
        defines { "_UNICODE",
                    "PHYSFS_SUPPORTS_GRP=0", "PHYSFS_SUPPORTS_HOG=0",
                    "PHYSFS_SUPPORTS_MVL=0", "PHYSFS_SUPPORTS_WAD=0",
                    "PHYSFS_SUPPORTS_QPAK=0", "PHYSFS_SUPPORTS_SLB=0",
                    "PHYSFS_SUPPORTS_ISO9660=0", "PHYSFS_SUPPORTS_VDF=0",
                    "PHYSFS_SUPPORTS_7Z=0", "PHYSFS_NO_CDROM_SUPPORT=1", }
        flags { "StaticRuntime", --[["Unicode"]] }

        configuration ( "Release" )
            optimize "On"
            objdir ( "objs/rlz/" )
            targetdir ( "bin/rlz/" )
            defines { "NDEBUG", "_NDEBUG" }

        configuration ( "Debug" )
            symbols "On"
            objdir ( "objs/dbg/" )
            targetdir ( "bin/dbg/" )
            defines { "DEBUG", "_DEBUG" }

        configuration ( "vs*" )
            excludes { "./src/libraries/luasocket/src/usocket.*",
                        "./src/libraries/luasocket/src/unix.*" }
            defines { "WIN32", "_WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE", "_CRT_NONSTDC_NO_DEPRECATE", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
            editandcontinue "Off"

        configuration ( "gmake" )
            symbols "On"
            excludes { "./src/libraries/luasocket/src/wsocket.*" }
            defines { "LINUX_OR_MACOSX" }
            buildoptions { "-fPIC" }
            linkoptions { "-rdynamic" }

        configuration { "gmake", "macosx" }
            files { "./src/libraries/physfs/src/*.m" }
            defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh", "LUA_USE_MACOSX" }
            links { "Foundation.framework", "IOKit.framework" }

        configuration { "gmake", "linux" }
            defines { "__linux__", "LUA_USE_LINUX" }
            linkoptions { "-Wl,-E" }
            links { "m", "pthread", "dl" }
