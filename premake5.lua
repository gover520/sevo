-- A solution contains projects, and defines the available configurations
solution ( "sonic" )
    configurations { "Release", "Debug" }
    platforms { "x64" }

    if _ACTION == "clean" then
        os.rmdir(".vs")
        os.rmdir("bin")
        os.rmdir("objs")
        os.remove("sonic.VC.db")
    end

    -- A project defines one build target
    project ( "sonic" )
        kind ( "ConsoleApp" )
        language ( "C" )
        targetname ("sonic")
        includedirs { "./src", "./src/libraries/mclib", "./src/libraries/lua/src", "./src/libraries/mini-gmp" }
        --libdirs { "" }
        --links { "" }
        files { "./src/*.h", "./src/*.c",
                "./src/common/**.*", "./src/modules/**.*",
                "./src/libraries/lua/src/*.h",
                "./src/libraries/lua/src/*.c",
                "./src/libraries/mini-gmp/*.h",
                "./src/libraries/mini-gmp/*.c" }
        excludes { "./src/libraries/lua/src/lua.c",
                    "./src/libraries/lua/src/luac.c" }
        defines { "_UNICODE" }
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
            defines { "WIN32", "_WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE", "_CRT_NONSTDC_NO_DEPRECATE" }
            editandcontinue "Off"

        configuration ( "gmake" )
            symbols "On"
            defines { "LINUX_OR_MACOSX" }
            buildoptions { "-fPIC" }
            linkoptions { "-rdynamic" }

        configuration { "gmake", "macosx" }
            defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh", }
            links { "CoreFoundation.framework", "IOKit.framework" }

        configuration { "gmake", "linux" }
            defines { "__linux__" }
            links { "m", "pthread", "dl" }
