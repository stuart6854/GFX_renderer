workspace "PersonalRenderer"
    architecture "x64"
    targetdir "build"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

ExampleDir = "%{wks.location}" .. "/examples"
include "Dependencies.lua"

group "Dependencies"
    include "dependencies/glfw"
group ""

project "GFX"
    location "GFX"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/include/**.h",
        "%{prj.name}/include/**.cpp",

        "dependencies/fmt/src",
    }

    includedirs
    {
        "%{prj.name}/include",
        "%{prj.name}/src",

        "dependencies/fmt/include",

        "%{IncludeDir.Assimp}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.VulkanSDK}",
    }

    links
    {
        "GLFW",

        "%{Library.Vulkan}",
    }

    defines
    {
        "FMT_HEADER_ONLY",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
        "GLM_FORCE_LEFT_HANDED",
        "GFX_API_VULKAN",
    }

    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        symbols "On"

        links
        {
            "%{Library.Shaderc_Debug}",
            "%{Library.SPIRV_Cross_Debug}",
            "%{Library.SPIRV_Cross_GLSL_Debug}",
            "%{Library.SPIRV_Tools_Debug}",
            "%{Library.Assimp_Debug}",
        }

    filter "configurations:Release"
        optimize "on"

        defines
        {
            "NDEBUG"
        }

        links
        {
            "%{Library.Shaderc_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}",
            "%{Library.Assimp_Release}",
        }

function AddExample(example_name, src)
    project (example_name)
        location ("examples/" .. example_name)
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        staticruntime "off"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        debugdir "%{wks.location}"

        files
        {
            src,
            "examples/base/src/**.h",
            "examples/base/src/**.cpp",
        }

        includedirs
        {
            "examples/base/src",
            "GFX/include",
            "GFX/src",

            "dependencies/fmt/include",

            "%{IncludeDir.Assimp}",
            "%{IncludeDir.GLFW}",
            "%{IncludeDir.glm}",
            "%{IncludeDir.VulkanSDK}",
        }

        links
        {
            "GLFW",
            "GFX"
        }

        defines
        {
            "GLM_FORCE_DEPTH_ZERO_TO_ONE",
            "GLM_FORCE_LEFT_HANDED",
            "GFX_API_VULKAN",
        }

        filter "system:windows"
            systemversion "latest"
            
        filter "configurations:Debug"
            symbols "On"

            postbuildcommands
            {
                '{COPY} "%{wks.location}/dependencies/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"',
                '{COPY} "%{wks.location}/dependencies/VulkanSDK/Bin/shaderc_sharedd.dll" "%{cfg.targetdir}"',
            }

        filter "configurations:Release"
            optimize "on"

            defines
            {
                "NDEBUG"
            }

            postbuildcommands
            {
                '{COPY} "%{wks.location}/dependencies/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
            }
end

group "Examples"
    AddExample("HelloWindow", "examples/HelloWindow/HelloWindow.cpp")
    AddExample("HelloTriangle", "examples/HelloTriangle/HelloTriangle.cpp")
    AddExample("HelloUniforms", "examples/HelloUniforms/HelloUniforms.cpp")
    AddExample("HelloForwardRenderer", "examples/HelloForwardRenderer/HelloForwardRenderer.cpp")
group ""