#include <cx-engine/core/app.hpp>

#include <cx-engine/core/entity/components/multi-anim.hpp>
#include <cx-engine/core/entity/components/animation.hpp>

#include <cx-engine/core/app/config-drivers/default.hpp>

#include <cx-engine/systems/input/keyboard.hpp>
#include <cx-engine/systems/logging/logger.hpp>
#include <cx-engine/core/entity/entity.hpp>

#include <cx-engine/ui/text/builder.hpp>
#include <cx-engine/ui/text-browser.hpp>

using namespace cx::types;
using cx::Keyboard, cx::Key;
namespace ui = cx::ui;

enum class Direction { Down, Up, Left, Right };

class Player: public cx::Entity {
protected:
    sf::Clock clock;
    Direction dir = Direction::Down;

public:
    Player(HashMap<StringView, cx::Animation>&& anims) {
        addComponent<cx::MultiAnimComponent>(std::move(anims), "idle-down");
        setScale({ 10, 10 });
        setPosition({ 600.f, 800.f });
    }

    void update(float dt) override {
        auto* comp = getComponent<cx::MultiAnimComponent>();
        assert(comp != nullptr);

        sf::Vector2f velocity = { 0, 0 };
        const float speed = 300.f * dt;

        Direction lastDir = dir;

        bool moving = false;
        if (Keyboard::isPressed(Key::W)) velocity.y -= speed, dir = Direction::Up,    moving = true;
        if (Keyboard::isPressed(Key::S)) velocity.y += speed, dir = Direction::Down,  moving = true;
        if (Keyboard::isPressed(Key::A)) velocity.x -= speed, dir = Direction::Left,  moving = true;
        if (Keyboard::isPressed(Key::D)) velocity.x += speed, dir = Direction::Right, moving = true;

        if (lastDir != dir)
            switch (rand() % 5) {
            case 0: comp->setColor(sf::Color::Red);     break;
            case 1: comp->setColor(sf::Color::Blue);    break;
            case 2: comp->setColor(sf::Color::Green);   break;
            case 3: comp->setColor(sf::Color::White);   break;
            case 4: comp->setColor(sf::Color::Magenta); break;
            }

        StringView nextAnim;
        if (moving)
            switch (dir) {
            case Direction::Up:   nextAnim = "walk-up";    break;
            case Direction::Down: nextAnim = "walk-down";  break;
            default:              nextAnim = "walk-right"; break;
            }
        else
            switch (dir) {
            case Direction::Up:   nextAnim = "idle-up";    break;
            case Direction::Down: nextAnim = "idle-down";  break;
            default:              nextAnim = "idle-right"; break;
            }

        setPosition(getPosition() + velocity);
        comp->play(nextAnim);

        sf::Vector2f scale = { 7.f, 7.f };
        sf::FloatRect bounds = comp->getSprite().getLocalBounds();

        bool flip = dir == Direction::Left;
        if (flip) {
            scale.x = -7.f;
            setOrigin({ bounds.size.x, bounds.size.y / 2.f });
        } else {
            setOrigin({ 0.f, bounds.size.y / 2.f });
        }
        setScale(scale);

        const sf::Time interval = sf::seconds(1.0f / comp->getFps());
        if (clock.getElapsedTime() >= interval) {
            comp->setIndex((comp->getIndex() + 1) % comp->getFrameCount());
            clock.restart();
        }

        cx::Entity::update(dt);
    }
};

class Game: public cx::App {
public:
    cx::DefaultConfigDriver config;
    cx::CxpkArchive assets;

    ui::TextBrowser text;

    struct AnimState {
        cx::Entity ent;
        sf::Clock clock;
    };
    Vec<AnimState> ents;

    Optional<Player> player;

public:
    Game() : cx::App("Animation example") {}
    cx::ConfigDriver& getConfig() override {
        return config;
    }

    void init() override {
        assets.open("out/examples/animation.cxpk");

        ui::TextDocument doc = ui::TextDocumentBuilder({ .font = defaultFont, .size = 24, .color = sf::Color::White })
            .addPart("Press ")
            .setBold(true)
            .addPart("W, S, A or D")
            .setBold(false)
            .addPart(" to move your character")
            .build();

        text.setScale({ 1.5, 1.5 });
        text.setPosition({ 30, (float)config.getTargetResolution().y - 80 });
        text.setDocument(std::move(doc));

        HashMap<StringView, cx::Animation> anims;
        anims["idle-down"]  = assets.loadAnimation("idle-down.cxan");
        anims["idle-right"] = assets.loadAnimation("idle-right.cxan");
        anims["idle-up"]    = assets.loadAnimation("idle-up.cxan");
        anims["walk-down"]  = assets.loadAnimation("walk-down.cxan");
        anims["walk-right"] = assets.loadAnimation("walk-right.cxan");
        anims["walk-up"]    = assets.loadAnimation("walk-up.cxan");

        usize i = 0;
        for (const auto& [_, anim]: anims) {
            cx::Entity ent;
            ent.addComponent<cx::AnimationComponent>(anim);
            ent.setScale({ 10, 10 });
            ent.setPosition({ i++ * 200.f, 200.f });
            ents.push_back({ std::move(ent), sf::Clock() });
        }

        player = Player(std::move(anims));
        forceRecreateWindow(cx::WindowType::Normal);
    }

    void update(float dt) override {
        for (auto& [ent, clock]: ents) {
            auto* comp = ent.getComponent<cx::AnimationComponent>();
            assert(comp != nullptr);

            const sf::Time interval = sf::seconds(1.0f / comp->getFps());
            if (clock.getElapsedTime() >= interval) {
                comp->setIndex((comp->getIndex() + 1) % comp->getFrameCount());
                clock.restart();
            }
            ent.update(dt);
        }

        text.update(dt);
        player->update(dt);
    }

    void handle(const sf::Event& event) override {
        text.handle(event);
        player->handle(event);
    }

    void drawGame(sf::RenderTarget& target) override {
        for (auto& [ent, _]: ents) {
            target.draw(ent);
        }
        target.draw(*player);
        target.draw(text);
    }
};

int main() {
    Game game;
    return game.run();
}
