#pragma once

#include "GFX/Core/Base.h"
#include "ResourceSetLayout.h"

namespace gfx
{
    class UniformBuffer;
    class Texture;

    class ResourceSet
    {
    public:
        static auto Allocate(uint32_t frameIndex, uint32_t set, ResourceSetLayout* setLayout) -> OwnedPtr<ResourceSet>;
        static auto Create(uint32_t set, ResourceSetLayout* setLayout) -> OwnedPtr<ResourceSet>;

        virtual ~ResourceSet() = default;

        virtual void CopyBindings(const ResourceSet& other) = 0;

        virtual void SetUniformBuffer(uint32_t binding, UniformBuffer* buffer) = 0;
        virtual void SetTextureSampler(uint32_t binding, uint32_t index, Texture* texture) = 0;

        virtual void UpdateBindings() = 0;
    };
}
