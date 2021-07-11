#pragma once

#include "GFX/Core/Base.h"

namespace gfx
{
    enum class BufferUsage
    {
        eNone = 0,
        eStaging,
        eVertex,
        eIndex,
        eUniform
    };

    class Buffer
    {
    public:
        static auto Create(BufferUsage usage, uint64_t size, const void* data = nullptr) -> OwnedPtr<Buffer>;
        static auto CreateStaging(uint64_t size, const void* data = nullptr) -> OwnedPtr<Buffer>;
        static auto CreateVertex(uint64_t size, const void* data = nullptr) -> OwnedPtr<Buffer>;
        static auto CreateIndex(uint64_t size, const void* data = nullptr) -> OwnedPtr<Buffer>;

        virtual ~Buffer() = default;
    };
}
