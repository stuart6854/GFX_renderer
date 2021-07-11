#pragma once

#include "GFX/Core/Base.h"

#include <cstdint>
#include <vector>

namespace gfx
{
    class SwapChain;
    class Image;
    class RenderPass;

    class Framebuffer
    {
    public:
        static auto Create(SwapChain* swapChain) -> OwnedPtr<Framebuffer>;

        Framebuffer(SwapChain* swapChain)
            : m_swapChain(swapChain),
              m_isSwapChainTarget(true)
        {
        }

        virtual ~Framebuffer() = default;

        auto GetWidth() const -> uint32_t { return m_width; }
        auto GetHeight() const -> uint32_t { return m_height; }

        bool IsSwapChainTarget() const { return false; }
        auto GetSwapChain() const -> SwapChain* { return m_swapChain; }

        // auto GetDepthImage() const -> Image* { return m_depthAttachmentImage.get(); }

        virtual void Resize(uint32_t width, uint32_t height) = 0;

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
