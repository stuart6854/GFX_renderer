set(GFX_GRAPHICS_API "GFX_API_VULKAN" CACHE STRING "" FORCE)

macro(Add_Example example_name source)
	set(EXAMPLE_RESOURCES "")
	file(GLOB_RECURSE EXAMPLE_RESOURCES "${example_name}/resources/*.*")
	source_group("Resources" FILES ${EXAMPLE_RESOURCES})

    add_executable(${example_name} ${source} ${EXAMPLE_RESOURCES})
    target_link_libraries(${example_name} PRIVATE example_base gfx)

	set_target_properties (${example_name} PROPERTIES FOLDER Examples)

	add_custom_command(TARGET ${example_name} POST_BUILD
		# Copy Resources
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/${example_name}/resources/ ${CMAKE_CURRENT_BINARY_DIR}/resources/

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
Add_Example(HelloTriangle HelloTriangle/HelloTriangle.cpp)
Add_Example(HelloUniforms HelloUniforms/HelloUniforms.cpp)
Add_Example(HelloOffscreen HelloOffscreen/HelloOffscreen.cpp)
Add_Example(HelloForwardRenderer HelloForwardRenderer/HelloForwardRenderer.cpp)