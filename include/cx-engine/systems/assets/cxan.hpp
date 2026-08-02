#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>
#include <cx-engine/defs/fs.hpp>

#include <SFML/Graphics.hpp>

namespace cx {

CX_DEFINE_STANDARD_EXCEPTION(AnimationLoadError, ResourceException);

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

    explicit Animation(Slice<const byte> data) { load(data); }
    explicit Animation(const fs::path& file)   { open(file); }

    void open(const fs::path& file);
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
