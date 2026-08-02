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

class AnimationComponent: public SpriteComponent {
protected:
    Animation animation;
    uint index;

    template <typename TParam>
    AnimationComponent(Entity& parent, TParam&& param, const sf::Texture& texture, const sf::IntRect& rect)
        : SpriteComponent(parent, sf::Sprite(texture, rect))
        , animation(std::forward<TParam>(param))
        , index(0)
    {}

public:
    template <typename TParam = Animation>
    AnimationComponent(Entity& parent, TParam&& param)
        : AnimationComponent(parent, std::forward<TParam>(param), param.getFrame(0).texture, param.getFrame(0).rect)
    {}

    AnimationComponent(Entity& parent, CxpkArchive& archive, StringView path)
      : AnimationComponent(parent, archive.loadAnimation(path))
    {}

    uint getIndex() { return index; }
    void setIndex(uint idx) { index = idx; }

    Animation& getAnimation() { return animation; }
    const Animation& getAnimation() const { return animation; }
    void setAnimation(Animation& anim) { animation = anim; }

    float getFps() const noexcept        { return animation.getFps(); }
    usize getFrameCount() const noexcept { return animation.getFrameCount(); }
    usize getPageCount() const noexcept  { return animation.getPageCount(); }

    void update(float dt) override {
        sprite.setTexture(animation.getFrame(index).texture);
        sprite.setTextureRect(animation.getFrame(index).rect);
    }

private:
    void setTexture(const sf::Texture& texture, bool resetRect = false) override {}
    void setTextureRect(const sf::IntRect& rect) override {}
};

} // namespace cx
