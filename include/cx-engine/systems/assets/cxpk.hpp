#pragma once

#include <cx-engine/defs/types.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cx-engine/systems/assets/cxsi.hpp>
#include <filesystem>

#include <cx-engine/systems/assets/cxpk/error.hpp>
#include <cx-engine/systems/assets/cxpk/header-parser.hpp>

#include <cx-engine/systems/assets/cxpk/backend.hpp>

#if CXPK_BACKEND == BACKEND_MEM
#   include <cx-engine/systems/assets/cxpk/mem-backend.hpp>
#elif CXPK_BACKEND == BACKEND_POSIX
#   include <cx-engine/systems/assets/cxpk/posix-backend.hpp>
#elif CXPK_BACKEND == BACKEND_WIN
#   include <cx-engine/systems/assets/cxpk/windows-backend.hpp>
#else
#   error "Invalid CXPK Backend ID. Use 1 for mem, 2 for posix and 3 for windows"
#endif

#include <cx-engine/systems/assets/cxmp.hpp>
#include <cx-engine/systems/assets/cxta.hpp>

namespace cx {

class CxpkArchive {
private:
    CxpkBackendImpl backend;

public:
    CxpkArchive();
    explicit CxpkArchive(const std::filesystem::path& filePath);
    ~CxpkArchive();

    void open(const std::filesystem::path& filePath);
    bool isOpen() const noexcept;

    bool has(StringView path) const;
    Slice<const byte> getBytes(StringView path) const;

    sf::Texture loadTexture(StringView path) const;
    sf::SoundBuffer loadSound(StringView path) const;
    sf::Music loadMusic(StringView path) const;
    sf::Font loadFont(StringView path) const;

    sf::Shader loadShader(StringView path, sf::Shader::Type type) const;
    sf::Shader loadShader(StringView vertShaderPath, StringView fragShaderPath) const;

    ScaledImage loadScaledImage(StringView path) const;
    TileMap loadTileMap(StringView path) const;
    TextureAtlas loadTextureAtlas(StringView path) const;
};

} // namespace cx
