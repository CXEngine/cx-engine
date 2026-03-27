#include <cx-engine/systems/assets/cxpk.hpp>

namespace cx {

CxpkArchive::CxpkArchive() = default;
CxpkArchive::CxpkArchive(const std::filesystem::path& filePath) { open(filePath); }
CxpkArchive::~CxpkArchive() = default;

void CxpkArchive::open(const std::filesystem::path& filePath) {
    backend.loadFromFile(filePath);
}

bool CxpkArchive::isOpen() const noexcept {
    return backend.isOpen();
}

bool CxpkArchive::has(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("Archive is not loaded");
    return backend.has(path);
}

Slice<const byte> CxpkArchive::getBytes(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    return backend.getBytes(path);
}

sf::Texture CxpkArchive::loadTexture(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto bytes = getBytes(path);

    sf::Texture tex;
    if (!tex.loadFromMemory(bytes.data(), bytes.size()))
        throw CxpkLoadError("failed to load sfml texture: " + String(path));

    return tex;
}

ScaledImage CxpkArchive::loadScaledImage(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto bytes = getBytes(path);

    ScaledImage img;
    img.load(bytes);
    return img;
}

sf::SoundBuffer CxpkArchive::loadSound(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto bytes = getBytes(path);

    sf::SoundBuffer buffer;
    if (!buffer.loadFromMemory(bytes.data(), bytes.size()))
        throw CxpkLoadError("failed to load sfml sound: " + String(path));

    return buffer;
}

sf::Music CxpkArchive::loadMusic(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto bytes = getBytes(path);

    sf::Music music;
    if (!music.openFromMemory(bytes.data(), bytes.size()))
        throw CxpkLoadError("failed to open sfml music: " + String(path));

    return music;
}

sf::Font CxpkArchive::loadFont(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto bytes = getBytes(path);

    sf::Font font;
    if (!font.openFromMemory(bytes.data(), bytes.size()))
        throw CxpkLoadError("failed to load sfml font: " + String(path));

    return font;
}

TileMap CxpkArchive::loadTileMap(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto bytes = getBytes(path);

    TileMap map;
    map.load(bytes);
    return map;
}

TextureAtlas CxpkArchive::loadTextureAtlas(StringView path) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto bytes = getBytes(path);

    TextureAtlas atlas;
    atlas.load(bytes);
    return atlas;
}

sf::Shader CxpkArchive::loadShader(StringView path, sf::Shader::Type type) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto bytes = getBytes(path);
    auto sv = StringView((const char*) bytes.data(), bytes.size());

    sf::Shader shader;
    if (!shader.loadFromMemory(sv, type)) {
        throw CxpkLoadError("failed to load sfml shader from memory: " + String(path));
    }
    return shader;
}

sf::Shader CxpkArchive::loadShader(StringView vertShaderPath, StringView fragShaderPath) const {
    if (!isOpen()) throw CxpkLoadError("archive is not loaded");
    auto vertBytes = getBytes(vertShaderPath);
    auto fragBytes = getBytes(fragShaderPath);

    auto fragStringView = StringView((const char*) vertBytes.data(), vertBytes.size());
    auto vertStringView = StringView((const char*) fragBytes.data(), fragBytes.size());

    sf::Shader shader;
    if (!shader.loadFromMemory(vertStringView, fragStringView)) {
        throw CxpkLoadError("failed to load sfml shader from memory (vert: " + String(vertShaderPath) + ", frag: " + String(fragShaderPath) + ")");
    }
    return shader;
}

} // namespace cx
