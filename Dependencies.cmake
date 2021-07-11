find_package(Vulkan REQUIRED)

add_subdirectory(Dependencies/glfw)
set_target_properties (glfw PROPERTIES FOLDER "Dependencies/GLFW3")
set_target_properties (uninstall PROPERTIES FOLDER "Dependencies/GLFW3")
add_subdirectory(Dependencies/glm)
add_subdirectory(Dependencies/fmt)
set_target_properties (fmt PROPERTIES FOLDER Dependencies)
add_subdirectory(Dependencies/stb)

#find_library(assimp_Debug NAMES assimp-vc141-mtd PATHS "Dependencies/assimp/bin/Debug" REQUIRED)
find_library(assimp_Release NAMES assimp-vc141-mt PATHS "Dependencies/assimp/bin/Release" REQUIRED)
add_library(assimp STATIC IMPORTED GLOBAL)
set_target_properties(assimp PROPERTIES
	IMPORTED_LOCATION "${assimp_Release}"
	#IMPORTED_LOCATION "${assimp_Release}"
	#IMPORTED_LOCATION_DEBUG "${assimp_Debug}"
	#IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)
target_include_directories(assimp INTERFACE "Dependencies/assimp/include")

#find_library(shaderc_Debug NAMES shaderc_sharedd PATHS "Dependencies/VulkanSDK/Lib" REQUIRED)
find_library(shaderc_Release NAMES shaderc_shared PATHS "$ENV{VULKAN_SDK}/Lib" REQUIRED)
add_library(shaderc STATIC IMPORTED GLOBAL)
set_target_properties(shaderc PROPERTIES
	IMPORTED_LOCATION "${shaderc_Release}"
	#IMPORTED_LOCATION_RELEASE "${shaderc_Release}"
	#IMPORTED_LOCATION_DEBUG "${shaderc_Debug}"
	#IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)

find_library(spirv-cross-core_Debug NAMES spirv-cross-cored PATHS "Dependencies/VulkanSDK/Lib" REQUIRED)
find_library(spirv-cross-core_Release NAMES spirv-cross-core PATHS "$ENV{VULKAN_SDK}/Lib" REQUIRED)
add_library(spirv-cross-core STATIC IMPORTED GLOBAL)
set_target_properties(spirv-cross-core PROPERTIES
	IMPORTED_LOCATION "${spirv-cross-core_Release}"
	IMPORTED_LOCATION_DEBUG "${spirv-cross-core_Debug}"
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)

find_library(spirv-cross-glsl_Debug NAMES spirv-cross-glsld PATHS "Dependencies/VulkanSDK/Lib" REQUIRED)
find_library(spirv-cross-glsl_Release NAMES spirv-cross-glsl PATHS "$ENV{VULKAN_SDK}/Lib" REQUIRED)
add_library(spirv-cross-glsl STATIC IMPORTED GLOBAL)
set_target_properties(spirv-cross-glsl PROPERTIES
	IMPORTED_LOCATION "${spirv-cross-glsl_Release}"
	IMPORTED_LOCATION_DEBUG "${spirv-cross-glsl_Debug}"
	IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
)