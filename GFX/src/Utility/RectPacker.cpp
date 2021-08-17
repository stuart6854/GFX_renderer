#include "GFX/Utility/RectPacker.h"

#include "GFX/Debug.h"
#include "GFX/Resources/Texture.h"
#include "GFX/Utility/IO.h"

#include <algorithm>

namespace gfx
{
    RectPacker::RectPacker(uint32_t width, uint32_t height) : m_width(width), m_height(height) { Clear(); }

    void RectPacker::AddRect(int id, uint32_t width, uint32_t height) { m_rects.emplace_back(id, width, height); }

    bool RectPacker::Pack()
    {
        SortRects();

        int x = 0;
        int y = 0;
        int nextY = 0;
        for (auto& rect : m_rects)
        {
            bool packed = false;
            while (true)
            {
                // Test position current position
                if (m_isPacked[x][y] == false)
                {
                    if (PositionValid(x, y, rect.width, rect.height))
                    {
                        // Valid position, add rect
                        rect.x = x;
                        rect.y = y;
                        rect.isPacked = true;
                        m_packedRects[rect.id] = rect;

                        // Set area as packed
                        SetPacked(x, y, rect.width, rect.height);

                        if (y + rect.height > nextY) nextY = y + rect.height;

                        // Break inner loop, continue to next rect
                        packed = true;
                        break;
                    }
                }

                x++;
                if (x + rect.width >= m_width)
                {
                    // Move to next (scan)line
                    x = 0;
                    y = nextY;
                }
                if (y + rect.height >= m_height)
                {
                    // Failed to pack rect
                    packed = false;
                    break;
                }
            }
            // Lets return early if we fail to pack any rects
            if (!packed) return false;
        }

        // Success
        return true;
    }

    auto RectPacker::GetPackedRect(int id) -> const Rect& { return m_packedRects[id]; }

    void RectPacker::Clear()
    {
        m_rects.clear();
        m_packedRects.clear();
        m_isPacked.resize(m_width, std::vector<bool>(m_height, false));
    }

    void RectPacker::SortRects()
    {
        std::ranges::sort(m_rects, [this](const Rect& a, const Rect& b) { return a.height > b.height; });
    }

    bool RectPacker::PositionValid(int x, int y, int width, int height)
    {
        for (int xi = x; xi < x + width; xi++)
        {
            for (int yi = y; yi < y + height; yi++)
            {
                if (m_isPacked[xi][yi] == true) return false;
            }
        }

        return true;
    }

    void RectPacker::SetPacked(int x, int y, int width, int height)
    {
        for (int xi = x; xi < x + width; xi++)
        {
            for (int yi = y; yi < y + height; yi++)
            {
                m_isPacked[xi][yi] = true;
            }
        }
    }

}  // namespace gfx
