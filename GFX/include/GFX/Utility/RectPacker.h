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
        int width = 0;
        int height = 0;

        // Output
        int x = 0;
        int y = 0;

        bool isPacked = false;
    };

    class RectPacker
    {
    public:
        RectPacker(int width, int height);

        auto GetWidth() const -> int { return m_width; }
        auto GetHeight() const -> int { return m_height; }

        void AddRect(int id, int width, int height);
        bool Pack();

        auto GetPackedRect(int id) -> const Rect&;
        auto GetAllPackedRects() const -> const std::vector<Rect>& { return m_rects; }

        void Clear();

    private:
        void SortRects();
        bool PositionValid(int x, int y, int width, int height);
        void SetPacked(int x, int y, int width, int height);

    private:
        int m_width;
        int m_height;

        std::vector<Rect> m_rects;

        std::vector<std::vector<bool>> m_isPacked;

        std::map<int, Rect> m_packedRects;
    };

}  // namespace gfx
