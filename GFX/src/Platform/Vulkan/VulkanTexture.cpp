#include "VulkanTexture.h"

#include "VulkanCore.h"
#include "VulkanImage.h"

#include "GFX/Debug.h"
#include "GFX/DeviceContext.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace gfx
{
    Texture::Texture(DeviceContext& deviceCtx, const std::string& path, const TextureDesc& desc) : m_deviceCtx(deviceCtx), m_desc(desc)
    {
        int width, height, channels;

        if (stbi_is_hdr(path.c_str()))
        {
            m_imageData.Data = (uint8_t*)stbi_loadf(path.c_str(), &width, &height, &channels, 4);
            m_imageData.Size = width * height * 4 * sizeof(float);
            m_desc.Format = ImageFormat::eRGBA32F;
        }
        else
        {
            stbi_set_flip_vertically_on_load(1);
            m_imageData.Data = stbi_load(path.c_str(), &width, &height, &channels, 4);
            m_imageData.Size = width * height * 4;
            m_desc.Format = ImageFormat::eRGBA;
        }

        GFX_ASSERT(m_imageData.Data, "Failed to load image!");

        m_desc.Width = width;
        m_desc.Height = height;
        m_desc.Depth = 1;
        m_desc.Layers = 1;

        GFX_ASSERT(m_desc.Format != ImageFormat::eNone);

        Invalidate();
    }

    Texture::Texture(DeviceContext& deviceCtx, const TextureDesc& desc, const void* data) : m_deviceCtx(deviceCtx), m_desc(desc)
    {
        GFX_ASSERT(m_desc.Format == ImageFormat::eRGBA);

        const uint32_t size = m_desc.Width * m_desc.Height * 4;
        m_imageData = RawBuffer::Copy(data, size);

        Invalidate();
    }

    auto Texture::GetHash() const -> uint64_t { return m_image->GetHash(); }

    void Texture::Invalidate()
    {
        const auto device = Vulkan::GetDevice();
        const auto allocator = Vulkan::GetAllocator();

        if (m_image) m_image->Release();

        ImageDesc imageDesc{};
        imageDesc.Usage = m_desc.Usage;
        imageDesc.Format = m_desc.Format;
        imageDesc.Width = m_desc.Width;
        imageDesc.Height = m_desc.Height;
        imageDesc.Layers = m_desc.Layers;
        imageDesc.Mips = m_desc.Mips;
        m_image = std::make_shared<Image>(imageDesc);
        m_image->Invalidate();

        m_deviceCtx.Upload(this);
    }

}  // namespace gfx
