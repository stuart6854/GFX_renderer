#pragma once

#include "GFX/DeviceContext.h"
#include "Mesh.h"

#include <memory>

namespace gfx::Primitives
{
    auto CreatePlane(DeviceContext& deviceCtx, float size = 1.0f, const uint32_t resolution = 1) -> std::shared_ptr<Mesh>;

    // static auto CreateCone() -> std::shared_ptr<Mesh>;
    // static auto CreateCylinder() -> std::shared_ptr<Mesh>;
    // static auto CreateTorus() -> std::shared_ptr<Mesh>;

}  // namespace gfx::Primitives