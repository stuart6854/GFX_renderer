#pragma once

#include <memory>
#include <utility>

namespace gfx
{
    template <typename T>
    using SharedPtr = std::shared_ptr<T>;

    template <typename T>
    using OwnedPtr = std::unique_ptr<T>;

    template <typename T, typename... Args>
    constexpr SharedPtr<T> CreateShared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    constexpr OwnedPtr<T> CreateOwned(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
}
