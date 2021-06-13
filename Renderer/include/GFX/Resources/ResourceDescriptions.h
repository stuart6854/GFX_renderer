//
// Created by stumi on 11/06/21.
//

#ifndef PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H
#define PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H

#include <cstdint>

namespace gfx
{
    enum class BufferType
    {
        eNone = 0,
        eVertex,
        eIndex,
        eStaging
    };

    struct BufferDesc
    {
        BufferType Type;
        uint32_t Size;
        uint32_t Stride;
        uint32_t NumElements;
    };
}  // namespace gfx

#endif  // PERSONAL_RENDERER_RESOURCEDESCRIPTIONS_H
