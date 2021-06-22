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

#ifdef NDEBUG
    #define GFX_ASSERT(...)
#else
    #define GFX_EXPAND_VARGS(x) x

    #define GFX_ASSERT_NO_MESSAGE(condition)   \
        do                                     \
        {                                      \
            if (!(condition))                  \
            {                                  \
                GFX_ERROR("Assertion Failed"); \
                __debugbreak();                \
            }                                  \
        } while (0)
    #define GFX_ASSERT_MESSAGE(condition, ...)                   \
        do                                                       \
        {                                                        \
            if (!(condition))                                    \
            {                                                    \
                GFX_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
                __debugbreak();                                  \
            }                                                    \
        } while (0)

    #define GFX_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
    #define GFX_GET_ASSERT_MACRO(...) GFX_EXPAND_VARGS(GFX_ASSERT_RESOLVE(__VA_ARGS__, GFX_ASSERT_MESSAGE, GFX_ASSERT_NO_MESSAGE))

    #define GFX_ASSERT(...) GFX_EXPAND_VARGS(GFX_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__))
#endif
#endif  // PERSONAL_RENDERER_DEBUG_H
