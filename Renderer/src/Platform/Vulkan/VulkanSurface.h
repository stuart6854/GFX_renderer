//
// Created by stumi on 09/06/21.
//

#ifndef PERSONAL_RENDERER_VULKANSURFACE_H
#define PERSONAL_RENDERER_VULKANSURFACE_H

#include <vulkan/vulkan.hpp>

namespace gfx
{
    class ISurface
    {
    public:
        virtual auto CreateSurface(vk::Instance instance) -> vk::SurfaceKHR = 0;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_VULKANSURFACE_H
