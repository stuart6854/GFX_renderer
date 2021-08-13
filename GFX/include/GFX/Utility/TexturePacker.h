#pragma once

#include "GFX/Core/Base.h"
#include "GFX/Resources/TextureBuilder.h"
#include "GFX/Resources/TextureImporter.h"

#include <glm/vec2.hpp>

namespace gfx
{
    class Texture;

    class TexturePacker
    {
    public:
        TexturePacker(uint32_t textureSize, uint32_t channels, uint32_t spacing = 0);

        bool AddToPack(const std::vector<uint8_t>& data, uint32_t width, uint32_t height, glm::vec2& origin);
        bool AddToPack(const TextureBuilder& builder, glm::vec2& origin);
        bool AddToPack(const TextureImporter& importer, glm::vec2& origin);

        auto CreateTexture() const -> OwnedPtr<Texture>;
        void WriteToPng(const std::string& filename) const;

    private:
        bool IsPosValidForPendingTexture();
        void AddPendingTextureAtCurrentPosition();

        bool TryAddToPack();

        inline auto GetPackedIndex(uint32_t x, uint32_t y) const -> uint32_t;
        inline auto GetPackedDataIndex(uint32_t x, uint32_t y) const -> uint32_t;
        inline auto GetPendingDataIndex(uint32_t x, uint32_t y) const -> uint32_t;

    private:
        uint32_t m_textureSize = 0;
        uint32_t m_channels = 0;
        uint32_t m_spacing = 0;

        uint32_t m_x = 0;
        uint32_t m_y = 0;

        std::vector<uint8_t> m_packedData;
        std::vector<bool> m_packed;

        uint32_t m_pendingTextureWidth = 0;
        uint32_t m_pendingTextureHeight = 0;
        std::vector<uint8_t> m_pendingTextureData;
    };
}
