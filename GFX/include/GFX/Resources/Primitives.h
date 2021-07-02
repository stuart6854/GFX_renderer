#pragma once

#include "GFX/DeviceContext.h"
#include "Mesh.h"

#include <memory>

namespace gfx
{
    class DeviceContext;

    namespace Primitives
    {
        auto CreatePlane(DeviceContext& deviceCtx, float size = 1.0f, const uint32_t resolution = 1) -> std::shared_ptr<Mesh>;
        auto CreateBox(DeviceContext& deviceCtx, const glm::vec3& size = { 1.0f, 1.0f, 1.0f }) -> std::shared_ptr<Mesh>;

        // static auto CreateCone() -> std::shared_ptr<Mesh>;
        // static auto CreateCylinder() -> std::shared_ptr<Mesh>;
        // static auto CreateTorus() -> std::shared_ptr<Mesh>;
    } // namespace Primitives
}     // namespace gfx
