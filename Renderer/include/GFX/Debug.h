//
// Created by stumi on 17/06/21.
//

#ifndef PERSONAL_RENDERER_DEBUG_H
#define PERSONAL_RENDERER_DEBUG_H

#include <fmt/format.h>

#include <iostream>
#include <string>

#define GFX_INFO(x, ...)                        \
    do                                          \
    {                                           \
        auto msg = fmt::format(x, __VA_ARGS__); \
        std::cout << msg << std::endl;          \
    } while (0)

#define GFX_ERROR(x, ...)                       \
    do                                          \
    {                                           \
        auto msg = fmt::format(x, __VA_ARGS__); \
        std::cerr << msg << std::endl;          \
    } while (0)

#endif  // PERSONAL_RENDERER_DEBUG_H
