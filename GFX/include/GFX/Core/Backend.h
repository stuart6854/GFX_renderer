#pragma once

#include "Base.h"

namespace gfx
{
    enum class BackendType
    {
        eNone,
        eVulkan
    };

    class Backend
    {
    public:
        static auto Create(BackendType type, bool enableDebugLayer = false) -> OwnedPtr<Backend>;

        Backend(bool enableDebugLayer)
        {
        };
        virtual ~Backend() = default;

        virtual void WaitIdle() = 0;
    };
}
