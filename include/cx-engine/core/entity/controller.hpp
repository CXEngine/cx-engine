#pragma once
#include <cx-engine/core/entity/actor.hpp>

namespace cx {

struct ActorControllerContext {
    float dt;
};

class ActorController {
protected:
    Actor& target;

    using ControllerContext = ActorControllerContext;
    using ActionContext = Actor::ActionContext;
    using Ability = Actor::Ability;

public:
    ActorController(Actor& target) : target(target) {}
    virtual ~ActorController() = default;

    virtual void update(const ControllerContext&) = 0;
};

}
