#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics.hpp>

#include <filesystem>
#include <stdexcept>
#include <cstdint>

namespace cx {

class AnimationLoadError: public Exception {
public:
    using Exception::Exception;
};

struct AnimationPage {
    sf::Texture texture;
    u16 cols = 0;
    u16 rows = 0;
    u16 frameWidth = 0;
    u16 frameHeight = 0;
};

struct AnimationFrame {
    sf::Texture& texture;
    sf::IntRect rect;
};

class Animation {
private:
    float fps;

    Vec<AnimationPage> pages;
    Vec<Pair<u16, u16>> frameMap;

public:
    explicit Animation() = default;

    explicit inline Animation(const std::filesystem::path& file) { open(file); }
    explicit inline Animation(Slice<const byte> data)            { load(data); }

    void open(const std::filesystem::path& file);
    void load(Slice<const byte> data);

    float getFps() const noexcept { return fps; }

    inline usize getFrameCount() const noexcept {
        if (frameMap.empty()) return 0;
        return frameMap.size() / 2;
    }

    inline usize getPageCount() const noexcept {
        return pages.size();
    }

    const AnimationPage& getPage(uint16_t pageIndex) const;
    AnimationFrame getFrame(uint16_t index) const;
    sf::Sprite getSprite(uint16_t index) const;
};

} // namespace cx
