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
        static auto Create(BufferUsage usage, size_t size, const void* data = nullptr, bool forceLocalMemory = false) -> OwnedPtr<Buffer>;
        static auto CreateStaging(size_t size, const void* data = nullptr) -> OwnedPtr<Buffer>;
        static auto CreateVertex(size_t size, const void* data = nullptr, bool forceLocalMemory = false) -> OwnedPtr<Buffer>;
        static auto CreateIndex(size_t size, const void* data = nullptr, bool forceLocalMemory = false) -> OwnedPtr<Buffer>;
        static auto CreateUniform(size_t size, const void* data = nullptr) -> OwnedPtr<Buffer>;

        virtual ~Buffer() = default;

        virtual void SetData(size_t offset, size_t size, const void* data) = 0;
    };
}
