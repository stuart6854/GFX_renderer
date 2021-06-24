-- Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["Assimp"] = "%{wks.location}/dependencies/assimp/include"
IncludeDir["GLFW"] = "%{wks.location}/dependencies/glfw/include"
IncludeDir["glm"] = "%{wks.location}/dependencies/glm"
IncludeDir["stb"] = "%{wks.location}/dependencies/stb/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/dependencies/VulkanSDK/Lib"
LibraryDir["Assimp"] = "%{wks.location}/dependencies/assimp/bin/Release"
LibraryDir["Assimp_Debug"] = "%{wks.location}/dependencies/assimp/bin/Debug"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

Library["Shaderc_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"
Library["Assimp_Debug"] = "%{LibraryDir.Assimp_Debug}/assimp-vc141-mtd.lib"

Library["Shaderc_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
Library["Assimp_Release"] = "%{LibraryDir.Assimp}/assimp-vc141-mt.lib"