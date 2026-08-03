#include <cx-engine/core/entity/pawn.hpp>
#include <cx-engine/core/entity/actor.hpp>
#include <cx-engine/core/entity/controller.hpp>

namespace cx {

Pawn::Pawn(HybridPtr<Actor> actor, HybridPtr<ActorController> controller)
    : actor(std::move(actor)), controller(std::move(controller))
{
}

Pawn::Pawn(Pawn&& other) noexcept
    : actor(std::move(other.actor))
    , controller(std::move(other.controller))
{
    other.actor = nullptr;
    other.controller = nullptr;
}

Pawn& Pawn::operator=(Pawn&& other) noexcept {
    if (this != &other) {
        actor = std::move(other.actor);
        controller = std::move(other.controller);

        other.actor = nullptr;
        other.controller = nullptr;
    }
    return *this;
}

void Pawn::controllerUpdate(float dt) {
    if (controller != nullptr)
        controller->update(dt);
}
void Pawn::actorUpdate(float dt) {
    if (actor != nullptr)
        actor->update(dt);
}

void Pawn::controllerHandle(const sf::Event& event) {
    if (controller != nullptr)
        controller->handle(event);
}
void Pawn::actorHandle(const sf::Event& event) {
    if (actor != nullptr)
        actor->handle(event);
}

void Pawn::update(float dt) {
    controllerUpdate(dt), actorUpdate(dt);
}
void Pawn::handle(const sf::Event& event) {
    controllerHandle(event), actorHandle(event);
}

} // namespace cx
