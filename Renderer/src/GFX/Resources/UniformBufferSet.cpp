//
// Created by stumi on 20/06/21.
//
#include "GFX/Resources/UniformBufferSet.h"

namespace gfx
{
    UniformBufferSet::UniformBufferSet(uint32_t frames) : m_frames(frames) {}

    void UniformBufferSet::Create(uint32_t size, uint32_t binding)
    {
        for (uint32_t frame = 0; frame < m_frames; frame++)
        {
            auto uniformBuffer = std::make_shared<UniformBuffer>(size, binding);
            Set(uniformBuffer, 0, frame);
        }
    }

    auto UniformBufferSet::Get(uint32_t binding, uint32_t set, uint32_t frame) -> std::shared_ptr<UniformBuffer>
    {
        // TODO: Check (assert?) frame valid
        // TODO: Check (assert?) set valid
        // TODO: Check (assert?) binding valid

        return m_uniformBuffers.at(frame).at(set).at(binding);
    }

    void UniformBufferSet::Set(const std::shared_ptr<UniformBuffer>& uniformBuffer, uint32_t set, uint32_t frame)
    {
        m_uniformBuffers[frame][set][uniformBuffer->GetBinding()] = uniformBuffer;
    }

}  // namespace gfx