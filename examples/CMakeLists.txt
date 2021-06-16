macro(Add_Example example_name source)
    add_executable(${example_name} ${source})
    target_link_libraries(${example_name} PRIVATE example_base gfx)

    add_custom_command(TARGET ${example_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/${example_name}/resources/ $<TARGET_FILE_DIR:${example_name}>/resources/)
endmacro()

add_subdirectory(base)

Add_Example(HelloTriangle_Raw HelloTriangle_raw/HelloTriangle_raw.cpp)
Add_Example(HelloWindow HelloWindow/HelloWindow.cpp)
Add_Example(HelloTriangle HelloTriangle/HelloTriangle.cpp)
Add_Example(HelloUniforms HelloUniforms/HelloUniforms.cpp)