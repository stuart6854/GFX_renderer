#pragma once

#include "GFX/Core/Base.h"
#include "Shader.h"

#include <cstdint>
#include <map>

namespace gfx
{
    class Buffer;

    class UniformBuffer
    {
    public:
        UniformBuffer(size_t size, uint32_t binding);
        
        void SetData(size_t offset, size_t size, const void* data);

        auto GetBuffer() -> Buffer* { return m_buffer.get(); }
        auto GetBinding() const -> uint32_t { return m_binding; }

    private:
        OwnedPtr<Buffer> m_buffer;
        uint32_t m_binding;
    };

    class UniformBufferSet
    {
    public:
        static auto Create(uint32_t frames) -> OwnedPtr<UniformBufferSet>;

        UniformBufferSet(uint32_t frames);

        void Create(size_t size, uint32_t binding);

        auto Get(uint32_t binding, uint32_t set = 0, uint32_t frame = 0) -> UniformBuffer*;
        void Set(OwnedPtr<UniformBuffer>& uniformBuffer, uint32_t set = 0, uint32_t frame = 0);

    private:
        uint32_t m_frames;

        // frames->set->binding
        std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, OwnedPtr<UniformBuffer>>>> m_uniformBuffers;
    };
}
