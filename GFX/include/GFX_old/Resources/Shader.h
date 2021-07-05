//
// Created by stumi on 12/06/21.
//

#ifndef PERSONAL_RENDERER_SHADER_H
#define PERSONAL_RENDERER_SHADER_H

#include <string>
#include <memory>
#include <unordered_map>

#include "Platform/Vulkan/VulkanShader.h"

namespace gfxOld
{
    class ShaderLibrary
    {
    public:
        static void Add(const std::shared_ptr<Shader>& shader);
        static void Load(const std::string& path, bool forceCompile = false);
        static void Load(const std::string& name, const std::string& path);

        static auto Get(const std::string& name) -> const std::shared_ptr<Shader>&;

    private:
        static std::unordered_map<std::string, std::shared_ptr<Shader>> s_shaders;
    };
}

#endif  // PERSONAL_RENDERER_SHADER_H
