#pragma once

#include "GFX/Core/Base.h"
#include "ResourceSetLayout.h"

namespace gfx
{
    class UniformBuffer;

    class ResourceSet
    {
    public:
        static auto Create(uint32_t set, ResourceSetLayout* setLayout) -> OwnedPtr<ResourceSet>;

        virtual ~ResourceSet() = default;

        virtual void SetUniformBuffer(uint32_t binding, UniformBuffer* buffer) = 0;

        virtual void UpdateBindings() = 0;
    };
}
