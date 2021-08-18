#pragma once

#include <fmt/format.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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

    static DebugCallbackFn s_callback;

    void SetDebugCallback(DebugCallbackFn callback);

    template <typename... T>
    void Log(DebugLevel level, const std::string& msg, T&&... args)
    {
        if (s_callback != nullptr)
        {
            const auto output = fmt::format(msg, std::forward<T>(args)...);
            s_callback(level, output);
        }
    }
    template <typename... T>
    void Trace(const std::string& msg, T&&... args)
    {
        Log(DebugLevel::eTrace, msg, std::forward<T>(args)...);
    }

    template <typename... T>
    void Info(const std::string& msg, T&&... args)
    {
        Log(DebugLevel::eInfo, msg, std::forward<T>(args)...);
    }

    template <typename... T>
    void Warn(const std::string& msg, T&&... args)
    {
        Log(DebugLevel::eWarn, msg, std::forward<T>(args)...);
    }

    template <typename... T>
    void Error(const std::string& msg, T&&... args)
    {
        Log(DebugLevel::eError, msg, std::forward<T>(args)...);
    }
}  // namespace gfx

template <>
struct fmt::formatter<glm::vec2> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const glm::vec2 v, FormatContext& ctx)
    {
        return formatter<std::string>::format(fmt::format("({}, {})", v.x, v.y), ctx);
    }
};

template <>
struct fmt::formatter<glm::vec3> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const glm::vec3 v, FormatContext& ctx)
    {
        return formatter<std::string>::format(fmt::format("({}, {}, {})", v.x, v.y, v.z), ctx);
    }
};

template <>
struct fmt::formatter<glm::vec4> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const glm::vec4 v, FormatContext& ctx)
    {
        return formatter<std::string>::format(fmt::format("({}, {}, {}, {})", v.x, v.y, v.z, v.w), ctx);
    }
};

#define GFX_TRACE(...) ::gfx::Trace(__VA_ARGS__)

#define GFX_INFO(...) ::gfx::Info(__VA_ARGS__)

#define GFX_WARN(...) ::gfx::Warn(__VA_ARGS__)

#define GFX_ERROR(...) ::gfx::Error(__VA_ARGS__)

#ifdef NDEBUG
    #define GFX_ASSERT(...)
#else

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

    #define GFX_EXPAND_VARGS(x) x

    #define GFX_ASSERT(...) GFX_EXPAND_VARGS(GFX_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__))
#endif
