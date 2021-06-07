//
// Created by stumi on 07/06/21.
//

#ifdef GFX_API_VULKAN

    #include "GFX/GFX.h"

    #include <vulkan/vulkan.hpp>

    #include <iostream>

namespace gfx
{
    void Init()
    {
        std::cout << "Initialising Vulkan Renderer!" << std::endl;

        auto instanceVersion = vk::enumerateInstanceVersion();
        uint32_t major = VK_VERSION_MAJOR(instanceVersion);
        uint32_t minor = VK_VERSION_MINOR(instanceVersion);
        uint32_t patch = VK_VERSION_PATCH(instanceVersion);
        std::cout << "Vulkan version: " << major << "." << minor << "." << patch << std::endl;
    }

    void Shutdown() { std::cout << "Shutting down Vulkan Renderer!" << std::endl; }

}  // namespace gfx

#endif