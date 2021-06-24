//
// Created by stumi on 21/06/21.
//

#ifndef PERSONAL_RENDERER_RAWBUFFER_H
#define PERSONAL_RENDERER_RAWBUFFER_H

#include "GFX/Debug.h"

#include <cstdint>

namespace gfx
{
    struct RawBuffer
    {
        void* Data;
        uint32_t Size;

        RawBuffer() : Data(nullptr), Size(0) {}
        RawBuffer(void* data, uint32_t size) : Data(data), Size(size) {}

        static auto Copy(const void* src, uint32_t size) -> RawBuffer
        {
            RawBuffer buffer;
            buffer.Allocate(size);
            std::memcpy(buffer.Data, src, size);
            return buffer;
        }

        void Allocate(uint32_t size)
        {
            delete[] Data;
            Data = nullptr;

            if (size == 0) return;

            Data = new uint8_t[size];
            Size = size;
        }

        void Release()
        {
            delete[] Data;
            Data = nullptr;
            Size = 0;
        }

        void ZeroInitialise()
        {
            if (Data)
            {
                memset(Data, 0, Size);
            }
        }

        template <typename T>
        auto Read(uint32_t offset) -> T&
        {
            return *(T*)((uint8_t*)Data + offset);
        }

        void Write(void* data, uint32_t size, uint32_t offset = 0)
        {
            GFX_ASSERT(offset + size <= Size, "Buffer write overflow!");
            memcpy((uint8_t*)Data + offset, data, size);
        }

        operator bool() const { return Data; }

        auto operator[](int index) -> uint8_t& { return ((uint8_t*)Data)[index]; }
        auto operator[](int index) const -> uint8_t& { return ((uint8_t*)Data)[index]; }

        template <typename T>
        auto As() -> T*
        {
            return (T*)Data;
        }

        inline auto GetSize() const -> uint32_t { return Size; }
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_RAWBUFFER_H
