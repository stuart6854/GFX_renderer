#pragma once

#include "Backend.h"

namespace gfx
{
    bool Init(const BackendType& backendType, bool enableDebugLayer = false);
    void Shutdown();

    auto GetBackendType() -> BackendType;
    auto GetBackend() -> Backend*;
}
