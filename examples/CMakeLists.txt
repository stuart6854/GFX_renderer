macro(Add_Example example_name source)
    add_executable(${example_name} ${source})
    target_link_libraries(${example_name} PRIVATE example_base gfx)
endmacro()

add_subdirectory(base)

Add_Example(HelloTriangle_Raw HelloTriangle_raw/HelloTriangle_raw.cpp)
Add_Example(HelloWindow HelloWindow/HelloWindow.cpp)
Add_Example(HelloTriangle HelloTriangle/HelloTriangle.cpp)
Add_Example(HelloUniforms HelloUniforms/HelloUniforms.cpp)