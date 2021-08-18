#include "GFX/Resources/UniformBuffer.h"

#include "GFX/Resources/Buffer.h"

namespace gfx
{
    /* Uniform Buffer */

    UniformBuffer::UniformBuffer(const size_t size, const size_t binding) : m_binding(binding) { m_buffer = Buffer::CreateUniform(size); }

    void UniformBuffer::SetData(const size_t offset, const size_t size, const void* data) { m_buffer->SetData(offset, size, data); }

    /* Uniform Buffer Set */

    auto UniformBufferSet::Create(uint32_t frames) -> OwnedPtr<UniformBufferSet> { return CreateOwned<UniformBufferSet>(frames); }

    UniformBufferSet::UniformBufferSet(uint32_t frames) : m_frames(frames) {}

    void UniformBufferSet::Create(size_t size, uint32_t binding)
    {
        for (uint32_t frame = 0; frame < m_frames; frame++)
        {
            auto uniformBuffer = CreateOwned<UniformBuffer>(size, binding);
            Set(uniformBuffer, 0, frame);
        }
    }

    auto UniformBufferSet::Get(uint32_t binding, uint32_t set, uint32_t frame) -> UniformBuffer*
    {
        return m_uniformBuffers.at(frame).at(set).at(binding).get();
    }

    void UniformBufferSet::Set(OwnedPtr<UniformBuffer>& uniformBuffer, uint32_t set, uint32_t frame)
    {
        m_uniformBuffers[frame][set][uniformBuffer->GetBinding()] = std::move(uniformBuffer);
    }
}  // namespace gfx
