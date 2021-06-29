add_library(example_base 
    "src/ExampleBase/ExampleBase.cpp"
    "src/ExampleBase/ExampleBase.h"
    "src/ExampleBase/Window.cpp"
    "src/ExampleBase/Window.h"
    "src/ExampleBase/Input.cpp"
    "src/ExampleBase/Input.h"
)

target_include_directories(example_base PUBLIC src)

target_link_libraries(example_base PUBLIC gfx)