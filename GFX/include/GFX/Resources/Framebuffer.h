#pragma once

#include "GFX/Core/Base.h"
#include "Texture.h"

#include <glm/vec3.hpp>

#include <cstdint>
#include <vector>

namespace gfx
{
    class SwapChain;
    class Image;
    class RenderPass;

    struct FramebufferAttachmentDesc
    {
        TextureFormat Format;
    };

    struct FramebufferDesc
    {
        uint32_t Width = 0;
        uint32_t Height = 0;

        glm::vec4 ClearColor = { 0, 0, 0, 0 };
        bool ClearOnLoad = true;

        std::vector<FramebufferAttachmentDesc> Attachments;
        uint32_t Samples = 1; // Multisampling
    };

    class Framebuffer
    {
    public:
        static auto Create(SwapChain* swapChain) -> OwnedPtr<Framebuffer>;
        static auto Create(const FramebufferDesc& desc) -> OwnedPtr<Framebuffer>;

        virtual ~Framebuffer() = default;

        auto GetWidth() const -> uint32_t { return m_width; }
        auto GetHeight() const -> uint32_t { return m_height; }

        bool IsSwapChainTarget() const { return m_isSwapChainTarget; }
        auto GetSwapChain() const -> SwapChain* { return m_swapChain; }

        virtual auto GetColorAttachmentCount() const -> uint32_t = 0;

        virtual auto GetColorTexture(uint32_t index) const -> Texture* = 0;
        virtual auto GetDepthTexture() const -> Texture* = 0;

        virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;

    private:
        virtual void Invalidate() = 0;

    protected:
        bool m_isSwapChainTarget;
        SwapChain* m_swapChain;

        uint32_t m_width;
        uint32_t m_height;

        // std::vector<OwnedPtr<Image>> m_attachmentImages;
        // OwnedPtr<Image> m_depthAttachmentImage;

        SharedPtr<RenderPass> m_renderPass;
    };
}
