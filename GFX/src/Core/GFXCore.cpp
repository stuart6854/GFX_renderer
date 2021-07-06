#include "GFX/Core/GFXCore.h"

#include "GFX/Core/Base.h"

namespace gfx
{
    bool s_initialised = false;
    BackendType s_backendType = BackendType::eNone;
    OwnedPtr<Backend> s_backend = nullptr;

    bool Init(const BackendType& backendType, bool enableDebugLayer)
    {
        if (s_initialised)
        {
            // We have already been initialised
            return false;
        }

        s_backend = Backend::Create(backendType, enableDebugLayer);
        if (s_backend == nullptr)
        {
            // Failed to create backend
            return false;
        }

        s_backendType = backendType;
        s_initialised = true;

        return true;
    }

    void Shutdown()
    {
        // Destroy backend
        s_backend = nullptr;
        s_backendType = BackendType::eNone;
        // We are no longer initialised
        s_initialised = false;
    }

    auto GetBackendType() -> BackendType
    {
        return s_backendType;
    }

    auto GetBackend() -> Backend*
    {
        return s_backend.get();
    }
}
