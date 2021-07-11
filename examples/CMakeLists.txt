set(GFX_GRAPHICS_API "GFX_API_VULKAN" CACHE STRING "" FORCE)

macro(Add_Example example_name source)
    add_executable(${example_name} ${source})
    target_link_libraries(${example_name} PRIVATE example_base gfx)

	set_target_properties (${example_name} PROPERTIES FOLDER Examples)

	add_custom_command(TARGET ${example_name} POST_BUILD
		# Debug
		#COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Dependencies/VulkanSDK/Bin/shaderc_sharedd.dll ${CMAKE_CURRENT_BINARY_DIR}
		#COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Dependencies/assimp/bin/Debug/assimp-vc141-mtd.dll ${CMAKE_CURRENT_BINARY_DIR}
		# Release
		COMMAND ${CMAKE_COMMAND} -E copy $ENV{VULKAN_SDK}/Bin/shaderc_shared.dll ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/Dependencies/assimp/bin/Release/assimp-vc141-mt.dll ${CMAKE_CURRENT_BINARY_DIR}
	)
endmacro()

add_subdirectory(base)

Add_Example(HelloWindow HelloWindow/HelloWindow.cpp)
#Add_Example(HelloTriangle HelloTriangle/HelloTriangle.cpp)
#Add_Example(HelloUniforms HelloUniforms/HelloUniforms.cpp)
#Add_Example(HelloForwardRenderer HelloForwardRenderer/HelloForwardRenderer.cpp)