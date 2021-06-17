add_library(example_base src/ExampleBase/Window.cpp src/ExampleBase/Window.h ../HelloTriangle_raw/HelloTriangle_raw.cpp ../HelloTriangle_raw/HelloTriangle_raw.h)

target_include_directories(example_base PUBLIC src)

target_link_libraries(example_base PUBLIC gfx glfw glm)