#pragma once

#include "GFX/Core/Base.h"

namespace gfx
{
    class Buffer
    {
    public:
        static auto Create() -> OwnedPtr<Buffer>;

        virtual ~Buffer() = default;

    private:
    };
}
