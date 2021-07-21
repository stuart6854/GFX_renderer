#pragma once

#include <fmt/format.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <iostream>
#include <string>
#include <functional>

namespace gfx
{
    enum class DebugLevel
    {
        eTrace = 0,
        eInfo = 1,
        eWarn = 2,
        eError = 3
    };

    typedef std::function<void(DebugLevel, std::string)> DebugCallbackFn;

    void SetDebugCallback(DebugCallbackFn callback);

    void Log(DebugLevel level, const std::string& msg);
    void Trace(const std::string& msg);
    void Info(const std::string& msg);
    void Warn(const std::string& msg);
    void Error(const std::string& msg);
}

namespace fmt
{
    template <>
    struct formatter<glm::vec2>
    {
        template <typename ParseContext>
        constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const glm::vec2& r, FormatContext& ctx)
        {
            return format_to(ctx.out(), "{}, {}", r.x, r.y);
        }
    };

    template <>
    struct formatter<glm::vec3>
    {
        template <typename ParseContext>
        constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const glm::vec3& r, FormatContext& ctx)
        {
            return format_to(ctx.out(), "{}, {}, {}", r.x, r.y, r.z);
        }
    };

    template <>
    struct formatter<glm::vec4>
    {
        template <typename ParseContext>
        constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const glm::vec4& r, FormatContext& ctx)
        {
            return format_to(ctx.out(), "{}, {}, {}, {}", r.x, r.y, r.z, r.w);
        }
    };
}

#define GFX_TRACE(x, ...)                        \
    do                                          \
    {                                           \
        auto msg = fmt::format(x, __VA_ARGS__); \
        ::gfx::Trace(msg);                       \
    } while (0)

#define GFX_INFO(x, ...)                        \
    do                                          \
    {                                           \
        auto msg = fmt::format(x, __VA_ARGS__); \
        ::gfx::Info(msg);                       \
    } while (0)

#define GFX_WARN(x, ...)                        \
    do                                          \
    {                                           \
        auto msg = fmt::format(x, __VA_ARGS__); \
        ::gfx::Warn(msg);                       \
    } while (0)

#define GFX_ERROR(x, ...)                       \
    do                                          \
    {                                           \
        auto msg = fmt::format(x, __VA_ARGS__); \
        ::gfx::Error(msg);                      \
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
