#pragma once

#include <cx-engine/utils/hybrid-ptr.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Event.hpp>

namespace cx {

class Actor;
class ActorController;

struct ActorControllerContext;

class Pawn final: public sf::Drawable {
private:
    HybridPtr<Actor> actor;
    HybridPtr<ActorController> controller;

public:
    Pawn(Actor& actor, ActorController& controller);
    Pawn(HybridPtr<Actor> actor, HybridPtr<ActorController> controller);

    Pawn(Pawn&& other) noexcept;
    Pawn& operator=(Pawn&& other) noexcept;

    Pawn(const Pawn&) = delete;
    Pawn& operator=(const Pawn&) = delete;

    Actor* getActor() { return actor.get(); }
    ActorController* getController() { return controller.get(); }

    const Actor* getActor() const { return actor.get(); }
    const ActorController* getController() const { return controller.get();  }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void controllerUpdate(float dt);
    void actorUpdate(float dt);

    void controllerHandle(const sf::Event& event);
    void actorHandle(const sf::Event& event);

    void update(float dt);
    void handle(const sf::Event& event);
};

} // namespace cx
