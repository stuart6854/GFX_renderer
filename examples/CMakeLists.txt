macro(Add_Example example_name source)
    add_executable(${example_name} ${source})
    target_link_libraries(${example_name} PRIVATE GFX)
endmacro()

project(examples)

Add_Example(HelloWindow HelloWindow/HelloWindow.cpp)
Add_Example(HelloTriangle HelloTriangle/HelloTriangle.cpp)