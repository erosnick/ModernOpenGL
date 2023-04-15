--workspace: 对应VS中的解决方案
workspace "ModernOpenGL"
    configurations { "Debug", "Release" }    --解决方案配置项，Debug和Release默认配置
    location "."                             --解决方案文件夹，这是我自己喜欢用的project简写
    cppdialect "c++17"    
    startproject "ModernOpenGL"
    -- Turn on DPI awareness
    dpiawareness "High"

    --增加平台配置，我希望有Win32和x64两种平台
    platforms
    {
        "Win32",
        "x64"
    }

    --Win32平台配置属性
    filter "platforms:Win32"
        architecture "x86"      --指定架构为x86
        targetdir ("bin/%{cfg.buildcfg}_%{cfg.platform}/")      --指定输出目录
        objdir  ("obj/%{cfg.buildcfg}_%{cfg.platform}/")        --指定中间目录

    --x64平台配置属性
    filter "platforms:x64"
        architecture "x64"      --指定架构为x64
        targetdir ("bin/%{cfg.buildcfg}_%{cfg.platform}/")      --指定输出目录
        objdir  ("obj/%{cfg.buildcfg}_%{cfg.platform}/")        --指定中间目录

-- GLSLC helpers
dofile( "Utils/glslc.lua" )

project "Shaders"
	local shaders = { 
		"Assets/Shaders/*.vert",
		"Assets/Shaders/*.frag",
		"Assets/Shaders/*.comp",
		"Assets/Shaders/*.geom",
		"Assets/Shaders/*.tesc",
		"Assets/Shaders/*.tese"
	}

	kind "Utility"
	location "Assets/Shaders"

	files( shaders )

	handle_glsl_files( "-std=460core --target-env=opengl", "Assets/Shaders", "Assets/Shaders" )

--project: 对应VS中的项目
project "ModernOpenGL"
    kind "ConsoleApp"                       --项目类型，控制台程序
    language "C++"                          --工程采用的语言，Premake5.0当前支持C、C++、C#
    dependson { "Shaders" }
    location "Project"
    files 
    { 
        "src/**.h", 
        "src/**.cpp",
        './ThirdParty/GLAD/glad.c',
    }                                       --指定加载哪些文件或哪些类型的文件

    vpaths 
    {
        -- ["Headers/*"] = { "*.h", "*.hpp" },  --包含具体路径
        -- ["Sources/*"] = { "*.c", "*.cpp" },  --包含具体路径
        ["Headers"] = { "**.h", "**.hpp" },     --不包含具体路径
        ["Sources"] = { "**.c", "**.cpp" },     --不包含具体路径
        ["Docs"] = "**.md"
    }

    --Debug配置项属性
    filter "configurations:Debug"
        defines { "DEBUG", "FMT_HEADER_ONLY" }                 --定义Debug宏(这可以算是默认配置)
        symbols "On"                                           --开启调试符号
        debugdir "%{wks.location}"

        includedirs 
        { 
            './Src',
            './Src/Utils',
            './ThirdParty/stb',
            './ThirdParty/GLAD',
            './ThirdParty/imgui-1.89.2',
            './ThirdParty/tinyobjloader',
            './ThirdParty/Assimp/include',
            './ThirdParty/spdlog/include', 
            './ThirdParty/glm-0.9.9.8/glm',
            './ThirdParty/FreeType2/Includes',
            './ThirdParty/Optick_1.4.0/include',
            './ThirdParty/rapidobj-1.0.1/include',
            './ThirdParty/glfw-3.3.8.bin.WIN64/include',
            './ThirdParty/easy_profiler-v2.1.0-msvc15-win64/include',
        }

		libdirs 
        { 
            './ThirdParty/Assimp/lib/x64',
            './ThirdParty/FreeType2/Libs',
            './ThirdParty/glfw-3.3.8.bin.WIN64/lib-vc2022',
        }

		links 
        { 
            "ImGui",
            "glfw3.lib", 
            "freetype.lib",
            "assimp-vc143-mt.lib"
        }

    --Release配置项属性
    filter "configurations:Release"
        defines { "NDEBUG", "FMT_HEADER_ONLY" }                 --定义NDebug宏(这可以算是默认配置)
        optimize "On"                                           --开启优化参数
        debugdir "%{wks.location}"

        includedirs 
        { 
            './Src',
            './Src/Utils',
            './ThirdParty/stb',
            './ThirdParty/GLAD',
            './ThirdParty/imgui-1.89.2',
            './ThirdParty/Assimp/include',
            './ThirdParty/spdlog/include', 
            './ThirdParty/tinyobjloader',
            './ThirdParty/glm-0.9.9.8/glm',
            './ThirdParty/FreeType2/Includes',
            './ThirdParty/Optick_1.4.0/include',
            './ThirdParty/rapidobj-1.0.1/include',
            './ThirdParty/glfw-3.3.8.bin.WIN64/include',
            './ThirdParty/easy_profiler-v2.1.0-msvc15-win64/include',
        }

		libdirs 
        { 
            './ThirdParty/Assimp/lib/x64',
            './ThirdParty/FreeType2/Libs',
            './ThirdParty/glfw-3.3.8.bin.WIN64/lib-vc2022',
        }

		links 
        { 
            "ImGui",
            "glfw3.lib",
            "freetype.lib",
            "assimp-vc143-mt.lib"
        }

include "External.lua"