#include <cx-engine/core/app.hpp>

#include <cx-engine/core/entity/components/animation.hpp>
#include <cx-engine/core/app/config-drivers/default.hpp>
#include <cx-engine/systems/logging/logger.hpp>
#include <cx-engine/core/entity/entity.hpp>

#include <SFML/Graphics/RectangleShape.hpp>

using namespace cx::types;

static StringView anims[] = {
    "idle-down.cxan", "idle-right.cxan", "idle-up.cxan",
    "walk-down.cxan", "walk-right.cxan", "walk-up.cxan",
};
constexpr usize AnimCount
    = sizeof(anims) / sizeof(StringView);

class Game: public cx::App {
public:
    cx::DefaultConfigDriver config;
    cx::CxpkArchive assets;

    struct AnimState {
        cx::Entity ent;
        sf::Clock clock;
    };
    cx::Buf<AnimState, AnimCount> ents;

public:
    Game() : cx::App("Animation example") {}
    cx::ConfigDriver& getConfig() override {
        return config;
    }

    void init() override {
        assets.open("out/examples/animation.cxpk");

        for (usize i = 0; i < AnimCount; ++i) {
            cx::Entity ent;
            ent.addComponent<cx::AnimationComponent>(assets, anims[i]);
            ent.setScale({ 10, 10 });
            ent.setPosition({ i * 200.f, 200.f });
            ents.push({ std::move(ent), sf::Clock() });
        }
        forceRecreateWindow(cx::WindowType::Normal);
    }

    void update(float dt) override {
        for (auto& [ent, clock] : ents) {
            auto* comp = ent.getComponent<cx::AnimationComponent>();
            assert(comp != nullptr);

            const sf::Time interval = sf::seconds(1.0f / comp->getFps());
            if (clock.getElapsedTime() >= interval) {
                comp->setIndex((comp->getIndex() + 1) % comp->getFrameCount());
                clock.restart();
            }
            ent.update(dt);
        }
    }

    void drawGame(sf::RenderTarget& target) override {
        for (auto& [ent, _] : ents) {
            target.draw(ent);
        }
    }
};

int main() {
    Game game;
    return game.run();
}
