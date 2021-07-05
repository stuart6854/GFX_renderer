#pragma once

#include "Backend.h"

namespace gfx
{
    bool Init(const BackendType& backend);
    void Shutdown(const BackendType& backend);

    auto GetBackendType() -> BackendType;
    auto GetBackend() -> Backend*;
}
