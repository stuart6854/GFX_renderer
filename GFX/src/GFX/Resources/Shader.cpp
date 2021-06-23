#include "GFX/Resources/Shader.h"

#include "GFX/Debug.h"

namespace gfx
{
    std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::s_shaders;

    void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
    {
        auto& name = shader->GetName();
        GFX_ASSERT(s_shaders.find(name) == s_shaders.end());
        s_shaders[name] = shader;
    }

    void ShaderLibrary::Load(const std::string& path, bool forceCompile)
    {
        const auto shader = std::make_shared<Shader>(path, forceCompile);
        auto& name = shader->GetName();
        GFX_ASSERT(s_shaders.find(name) == s_shaders.end());
        s_shaders[name] = shader;
    }

    void ShaderLibrary::Load(const std::string& name, const std::string& path)
    {
        GFX_ASSERT(s_shaders.find(name) == s_shaders.end());
        s_shaders[name] = std::make_shared<Shader>(path);
    }

    auto ShaderLibrary::Get(const std::string& name) -> const std::shared_ptr<Shader>&
    {
        GFX_ASSERT(s_shaders.find(name) != s_shaders.end());
        return s_shaders[name];
    }

}  // namespace gfx
