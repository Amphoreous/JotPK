newoption
{
    trigger = "graphics",
    value = "OPENGL_VERSION",
    description = "version of OpenGL to build raylib against",
    allowed = {
        { "opengl11", "OpenGL 1.1"},
        { "opengl21", "OpenGL 2.1"},
        { "opengl33", "OpenGL 3.3"},
        { "opengl43", "OpenGL 4.3"},
        { "openges2", "OpenGL ES2"},
        { "openges3", "OpenGL ES3"}
    },
    default = "opengl33"
}

function download_progress(total, current)
    local ratio = current / total
    ratio = math.min(math.max(ratio, 0), 1)
    local percent = math.floor(ratio * 100)
    print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
    os.chdir("external")
    if not os.isdir("raylib-master") then
        if not os.isfile("raylib-master.zip") then
            print("Raylib not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "raylib-master.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " .. os.getcwd())
        zip.extract("raylib-master.zip", os.getcwd())
        os.remove("raylib-master.zip")
    end
    os.chdir("../")
end

function check_discord_sdk()
    os.chdir("external")
    
    -- Create discord_game_sdk directory if it doesn't exist
    if not os.isdir("discord_game_sdk") then
        os.mkdir("discord_game_sdk")
    end

    -- Change to discord_game_sdk directory
    os.chdir("discord_game_sdk")
    
    -- Check if lib folder exists
    if not os.isdir("lib") then
        -- Check for zip file
        if not os.isfile("discord_game_sdk.zip") then
            print("Discord SDK not found, downloading from discord.com")
            local result_str, response_code = http.download(
                "https://dl-game-sdk.discordapp.net/2.5.6/discord_game_sdk.zip", 
                "discord_game_sdk.zip", 
                {
                    progress = download_progress,
                    headers = { "From: Premake", "Referer: Premake" }
                }
            )
        end
        
        -- Unzip the file
        print("Unzipping to " .. os.getcwd())
        zip.extract("discord_game_sdk.zip", os.getcwd())
        os.remove("discord_game_sdk.zip")
    end
    
    -- Return to original directory
    os.chdir("../../")
end
function build_externals()
    print("calling externals")
    check_raylib()
    check_discord_sdk()
end

function platform_defines()
    filter {"configurations:Debug or Release"}
        defines{"PLATFORM_DESKTOP"}

    filter {"configurations:Debug_RGFW or Release_RGFW"}
        defines{"PLATFORM_DESKTOP_RGFW"}

    filter {"options:graphics=opengl43"}
        defines{"GRAPHICS_API_OPENGL_43"}

    filter {"options:graphics=opengl33"}
        defines{"GRAPHICS_API_OPENGL_33"}

    filter {"options:graphics=opengl21"}
        defines{"GRAPHICS_API_OPENGL_21"}

    filter {"options:graphics=opengl11"}
        defines{"GRAPHICS_API_OPENGL_11"}

    filter {"options:graphics=openges3"}
        defines{"GRAPHICS_API_OPENGL_ES3"}

    filter {"options:graphics=openges2"}
        defines{"GRAPHICS_API_OPENGL_ES2"}

    filter {"system:macosx"}
        disablewarnings {"deprecated-declarations"}

    filter {"system:linux"}
        defines {"_GLFW_X11"}
        defines {"_GNU_SOURCE"}
    filter{}
end

-- if you don't want to download raylib, then set this to false, and set the raylib dir to where you want raylib to be pulled from, must be full sources.
downloadRaylib = true
raylib_dir = "external/raylib-master"

workspaceName = 'MyGame'
baseName = path.getbasename(path.getdirectory(os.getcwd()))

--if (baseName ~= 'raylib-quickstart') then
    workspaceName = baseName
--end

if (os.isdir('build_files') == false) then
    os.mkdir('build_files')
end

if (os.isdir('external') == false) then
    os.mkdir('external')
end

workspace (workspaceName)
    location "../"
    configurations { "Debug", "Release", "Debug_RGFW", "Release_RGFW"}
    platforms { "x64", "x86", "ARM64"}

    defaultplatform ("x64")

    filter "configurations:Debug or Debug_RGFW"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release or Release_RGFW"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "platforms:Arm64" }
        architecture "ARM64"

    filter {}

    targetdir "bin/%{cfg.buildcfg}/"

if (downloadRaylib) then
    build_externals()
end

    startproject(workspaceName)

    project (workspaceName)
        kind "ConsoleApp"
        location "build_files/"
        targetdir "../bin/%{cfg.buildcfg}"

        filter {"system:windows", "configurations:Release or Release_RGFW", "action:gmake*"}
            kind "WindowedApp"
            buildoptions { "-Wl,--subsystem,windows" }

        filter {"system:windows", "configurations:Release or Release_RGFW", "action:vs*"}
            kind "WindowedApp"
            entrypoint "mainCRTStartup"

        filter "action:vs*"
            debugdir "$(SolutionDir)"

        filter{}

        vpaths 
        {
            ["Header Files/*"] = { "../include/**.h",  "../include/**.hpp", "../src/**.h", "../src/**.hpp"},
            ["Source Files/*"] = {"../src/**.c", "../src/**.cpp"},
            ["Resource Files/*"] = {"../src/**.rc", "../src/*.ico"}  -- Added resource files path
        }
        
        files {
            "../src/**.c", 
            "../src/**.cpp", 
            "../src/**.h", 
            "../src/**.hpp", 
            "../include/**.h", 
            "../include/**.hpp"
        }
        
        -- Add Windows-specific resource files
        filter {"system:windows"}
            files {"../src/*.rc", "../src/*.ico"}
            
        -- Handle resource compilation for MinGW
        filter {"system:windows", "action:gmake*", "files:**.rc"}
            buildmessage 'Compiling Windows Resources %{file.relpath}'
            buildcommands {
                'windres "%{file.relpath}" "%{cfg.objdir}/%{file.basename}.o"'
            }
            buildoutputs {
                '%{cfg.objdir}/%{file.basename}.o'
            }

        filter {}

        -- Exclude Discord manager implementation files
        removefiles {
            "../src/discord/**.cpp", 
            "../src/discord/**.h", 
            "../include/discord/**.h", 
            "../include/discord/**.hpp"
        }

        includedirs { "../src" }
        includedirs { "../include" }

        links { "raylib", "discord_game_sdk" }

        cdialect "C17"
        cppdialect "C++17"

        includedirs {raylib_dir .. "/src" }
        includedirs {raylib_dir .."/src/external" }
        includedirs { raylib_dir .."/src/external/glfw/include" }
        flags { "ShadowedVariables"}
        platform_defines()

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            dependson {"raylib"}
            libdirs {"../build/external/discord_game_sdk/lib/x86_64"}
            links {"raylib.lib", "discord_game_sdk.dll.lib"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }

        filter "system:windows"
            defines{"_WIN32"}
            links {"winmm", "gdi32", "opengl32"}
            libdirs {"../bin/%{cfg.buildcfg}"}
            postbuildcommands {
                "{COPY} ../external/discord_game_sdk/lib/x86_64/discord_game_sdk.dll %{cfg.targetdir}",
                "{COPY} ../external/discord_game_sdk/lib/x86_64/discord_game_sdk.dll.lib %{cfg.targetdir}"
            }

        filter "system:linux"
            links {"pthread", "m", "dl", "rt", "X11"}

        filter "system:macosx"
            links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}

        filter{}
        
    project "raylib"
        kind "StaticLib"
    
        platform_defines()

        location "build_files/"

        language "C"
        targetdir "../bin/%{cfg.buildcfg}"

        filter "action:vs*"
            defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
            characterset ("Unicode")
            buildoptions { "/Zc:__cplusplus" }
        filter{}

        includedirs {raylib_dir .. "/src", raylib_dir .. "/src/external/glfw/include" }
        vpaths
        {
            ["Header Files"] = { raylib_dir .. "/src/**.h"},
            ["Source Files/*"] = { raylib_dir .. "/src/**.c"},
        }
        files {raylib_dir .. "/src/*.h", raylib_dir .. "/src/*.c"}

        removefiles {raylib_dir .. "/src/rcore_*.c"}

        filter { "system:macosx", "files:" .. raylib_dir .. "/src/rglfw.c" }
            compileas "Objective-C"

        filter{}

-- Add this after the raylib project section

project "discord_game_sdk"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
    location "build_files/"
    targetdir "../bin/%{cfg.buildcfg}"
    
    -- Define paths
    local discord_sdk_dir = "external/discord_game_sdk"
    
    -- Include directories
    includedirs { "../include" }
    includedirs { discord_sdk_dir .. "/cpp" }
    
    -- Set up source files
    vpaths 
    {
        ["Header Files"] = { 
            discord_sdk_dir .. "/cpp/**.h",
            "../include/discord/**.h"
        },
        ["Source Files"] = { 
            discord_sdk_dir .. "/cpp/**.cpp",
            "../src/discord/**.cpp" 
        },
    }
    
    -- Add files - SDK header files + our wrapper implementation
    files { 
        discord_sdk_dir .. "/cpp/**.h", 
        discord_sdk_dir .. "/cpp/**.cpp",
        "../include/discord/**.h",
        "../src/discord/**.cpp"
    }
    
    -- Platform-specific settings
    filter "action:vs*"
        defines { "_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS" }
        characterset ("Unicode")
        buildoptions { "/Zc:__cplusplus" }
        
    filter "system:windows"
        defines { "_WIN32" }
        
    filter {}