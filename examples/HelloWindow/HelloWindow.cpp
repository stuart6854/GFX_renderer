//
// Created by stumi on 07/06/21.
//

#include <GFX/GFX.h>

#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "Running example \"HelloWindow\"" << std::endl;

    gfx::Init();

    gfx::Shutdown();

    return 0;
}