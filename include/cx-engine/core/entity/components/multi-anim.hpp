#pragma once

#include <cx-engine/core/entity/components/sprite.hpp>
#include <cx-engine/core/entity/component.hpp>
#include <cx-engine/core/entity/entity.hpp>
#include <cx-engine/defs/types.hpp>

#include <cx-engine/systems/assets/cxpk.hpp>
#include <cx-engine/systems/assets/cxan.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transform.hpp>

namespace cx {

class MultiAnimComponent: public SpriteComponent {
protected:
    HashMap<StringView, Animation> animations;
    HashMap<StringView, uint> frameIndices;
    StringView currentAnimation;

public:
    MultiAnimComponent(
        Entity& parent, HashMap<StringView, Animation>&& animations, StringView initialAnimation
    )
        : SpriteComponent(
            parent, animations.at(initialAnimation).getFrame(0).texture,
            animations.at(initialAnimation).getFrame(0).rect
          )
        , animations(std::move(animations))
        , currentAnimation(initialAnimation)
    {
        for (auto& [name, anim] : this->animations) {
            frameIndices[name] = 0;
        }
    }

    void addAnimation(StringView name, Animation&& anim) {
        StringView key(name);
        animations[key] = std::move(anim);
        frameIndices[key] = 0;
    }

    void play(StringView name, bool preserveState = true) {
        if (animations.find(name) == animations.end()) return;
        if (!preserveState) frameIndices[name] = 0;
        currentAnimation = name;
    }

    void setIndex(uint idx) {
        if (!currentAnimation.empty()) frameIndices[currentAnimation] = idx;
    }

    uint getIndex() const {
        if (currentAnimation.empty()) return 0;
        return frameIndices.at(currentAnimation);
    }

    Animation& getAnimation() { return animations.at(currentAnimation); }
    const Animation& getAnimation() const { return animations.at(currentAnimation); }

    float getFps() const noexcept { return animations.at(currentAnimation).getFps(); }
    usize getFrameCount() const noexcept { return animations.at(currentAnimation).getFrameCount(); }

    void update(float dt) override {
        if (currentAnimation.empty()) return;
        auto& anim = animations.at(currentAnimation);
        uint index = frameIndices.at(currentAnimation);

        sprite.setTexture(anim.getFrame(index).texture);
        sprite.setTextureRect(anim.getFrame(index).rect);
    }

private:
    void setTexture(const sf::Texture& texture, bool resetRect = false) override {}
    void setTextureRect(const sf::IntRect& rect) override {}
};

} // namespace cx
