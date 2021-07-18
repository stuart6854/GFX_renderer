#pragma once

#include "GFX/Core/Base.h"
#include "ResourceSetLayout.h"

namespace gfx
{
    class ResourceSet
    {
    public:
        static auto Create(ResourceSetLayout* setLayout) -> OwnedPtr<ResourceSet>;

        virtual ~ResourceSet() = default;
    };
}
