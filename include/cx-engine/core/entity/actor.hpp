#pragma once
#include <cx-engine/core/entity/entity.hpp>
#include <cx-engine/core/entity/team.hpp>

#include <cx-engine/defs/types.hpp>

#include <SFML/System/Angle.hpp>

namespace cx {

enum class ActionResult {
    Success,
    NoHandlerFound,
    NoSuchAbility,
};

enum class ActorAbility {
    Offensive,
    Deffensive,
    Tactical,
    Critical,

    ShootWeapon,
    ReloadWeapon,
};

class ActorActionContext {
public:
    float dt;
};

class Actor: public Entity {
public:
    using ActionContext = ::cx::ActorActionContext;
    using ActionResult = ::cx::ActionResult;

    using Ability = ::cx::ActorAbility;

public:
    Actor(Actor&&) = default;
    using Entity::Entity;

    //virtual int getAvabileDashes();
    //virtual int getMaxDashes();

    //virtual int getCurrentWeaponIndex();
    //virtual int getWeaponsCount();

    virtual ActionResult move(const ActionContext&, sf::Vector2f dir)          { (void) dir; return ActionResult::NoHandlerFound; }
    virtual ActionResult dash(const ActionContext&, sf::Vector2f dir)          { (void) dir; return ActionResult::NoHandlerFound; }

    virtual ActionResult setAngle(const ActionContext&, sf::Angle angle)       { (void) angle; return ActionResult::NoHandlerFound; }
    virtual ActionResult rotate(const ActionContext&, sf::Angle delta)         { (void) delta; return ActionResult::NoHandlerFound; }

    virtual ActionResult switchWeapon(const ActionContext&, usize index)       { (void) index; return ActionResult::NoHandlerFound; }

    virtual ActionResult pressAbility(const ActionContext&, Ability ability)   { (void) ability; return ActionResult::NoHandlerFound; }
    virtual ActionResult releaseAbility(const ActionContext&, Ability ability) { (void) ability; return ActionResult::NoHandlerFound; }

    virtual ActionResult activateCore(const ActionContext&)                    { return ActionResult::NoHandlerFound; }

    virtual ActionResult embark(const ActionContext&)                          { return ActionResult::NoHandlerFound; }
    virtual ActionResult disembark(const ActionContext&)                       { return ActionResult::NoHandlerFound; }
};

} // namespace cx
