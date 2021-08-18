#pragma once

#include "GFX/Core/Base.h"
#include "Shader.h"

#include <cstdint>

namespace gfx
{
    enum class ResourceType
    {
        eNone = 0,
        eUniformBuffer,
        eTextureSampler
    };

    class ResourceSetLayout
    {
    public:
        static auto Create() -> OwnedPtr<ResourceSetLayout>;

        virtual ~ResourceSetLayout() = default;

        virtual void AddBinding(uint32_t binding, ResourceType type, size_t arraySize, ShaderStage stage) = 0;

        virtual void Build() = 0;
    };
}
