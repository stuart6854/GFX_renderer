#pragma once

#include <cstdint>
#include <vector>
#include <map>

namespace gfx
{
    struct Rect
    {
        int id = 0;

        // Input
        uint32_t width = 0;
        uint32_t height = 0;

        // Output
        uint32_t x = 0;
        uint32_t y = 0;

        bool isPacked = false;
    };

    class RectPacker
    {
    public:
        RectPacker(uint32_t width, uint32_t height);

        auto GetWidth() const -> int { return m_width; }
        auto GetHeight() const -> int { return m_height; }

        void AddRect(int id, uint32_t width, uint32_t height);
        bool Pack();

        auto GetPackedRect(int id) -> const Rect&;
        auto GetAllPackedRects() const -> const std::vector<Rect>& { return m_rects; }

        void Clear();

    private:
        void SortRects();
        bool PositionValid(int x, int y, int width, int height);
        void SetPacked(int x, int y, int width, int height);

    private:
        uint32_t m_width;
        uint32_t m_height;

        std::vector<Rect> m_rects;

        std::vector<std::vector<bool>> m_isPacked;

        std::map<int, Rect> m_packedRects;
    };

}  // namespace gfx
