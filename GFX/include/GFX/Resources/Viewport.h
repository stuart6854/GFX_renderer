#pragma once

namespace gfx
{
    struct Viewport
    {
        float X = 0;
        float Y = 0;
        float Width;
        float Height;
        float MinDepth = 0.0f;
        float MaxDepth = 1.0f;
    };
}
