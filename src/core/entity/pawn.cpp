#include <cx-engine/core/entity/pawn.hpp>
#include <cx-engine/core/entity/actor.hpp>
#include <cx-engine/core/entity/controller.hpp>

namespace cx {

Pawn::Pawn(HybridPtr<Actor> actor, HybridPtr<ActorController> controller)
    : actor(std::move(actor)), controller(std::move(controller))
{
}

Pawn::~Pawn() {}

Pawn::Pawn(Pawn&& other) noexcept
    : actor(std::move(other.actor)), controller(std::move(other.controller))
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

void Pawn::controllerUpdate(const ActorControllerContext& ctx) {
    if (controller != nullptr) {
        controller->update(ctx);
    }
}

void Pawn::actorUpdate(float dt) {
    if (actor != nullptr) {
        actor->update(dt);
    }
}

} // namespace cx
