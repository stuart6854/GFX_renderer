#include "GFX/Debug.h"

#include <utility>

namespace gfx
{
//    static DebugCallbackFn s_callback;

    void SetDebugCallback(DebugCallbackFn callback)
    {
        s_callback = std::move(callback);
    }

}
