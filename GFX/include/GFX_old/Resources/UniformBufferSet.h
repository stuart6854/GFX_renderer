//
// Created by stumi on 20/06/21.
//

#ifndef PERSONAL_RENDERER_UNIFORMBUFFERSET_H
#define PERSONAL_RENDERER_UNIFORMBUFFERSET_H

#include "UniformBuffer.h"

#include <cstdint>
#include <map>

namespace gfxOld
{
    class UniformBufferSet
    {
    public:
        UniformBufferSet(uint32_t frames);

        void Create(uint32_t size, uint32_t binding);

        auto Get(uint32_t binding, uint32_t set = 0, uint32_t frame = 0) -> std::shared_ptr<UniformBuffer>;
        void Set(const std::shared_ptr<UniformBuffer>& uniformBuffer, uint32_t set = 0, uint32_t frame = 0);

    private:
        uint32_t m_frames;
        std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, std::shared_ptr<UniformBuffer>>>> m_uniformBuffers;  // frame->set->binding
    };
}  // namespace gfxOld

#endif  // PERSONAL_RENDERER_UNIFORMBUFFERSET_H
