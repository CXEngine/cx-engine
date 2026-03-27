#include "cx-engine/systems/logging/logger.hpp"
#include "cx-engine/systems/settings/setting.hpp"
#include <cx-engine/core/entity/components/health.hpp>

#include <numeric>
#include <algorithm>

namespace cx {

Damage HealthLayer::takeDamage(Damage dmg) {
    if (dmg.type == DamageType::Chemical && !(flags & HealthLayerFlags::AllowsChem)) {
        return Damage(0, dmg.type);
    }

    if (dmg.type == DamageType::Electrical && bool(flags & HealthLayerFlags::EnergyBased)) {
        dmg.value += dmg.value / 2;
    }

    if (dmg.type == DamageType::Thermal && bool(flags & HealthLayerFlags::EnergyBased)) {
        return Damage(0, dmg.type);
    }

    HealthPoints wantedAbsorb = (HealthPoints) (dmg.value * absorb.ratio);
    HealthPoints cappedAbsorb = std::min(wantedAbsorb, absorb.maxAbsorb);
    HealthPoints actualAbsorb = std::min(cappedAbsorb, currentHp);

    currentHp -= actualAbsorb;
    Logger::debug("damage taken", {{"current-hp", toString(currentHp)}});

    HealthPoints overflow = dmg.value - actualAbsorb;

    return Damage(
        (HealthPoints) (overflow * overflowFactor),
        dmg.type
    );
}

HealthPoints HealthLayer::heal(HealthPoints amount, float overhealFactor) {
    HealthPoints maxAllowed =
        (HealthPoints) (maxHealth * (1.f + overhealFactor));

    if (currentHp >= maxAllowed)
        return 0;

    HealthPoints space = maxAllowed - currentHp;
    HealthPoints restored = std::min(space, amount);

    currentHp += restored;
    return restored;
}


DamageResult HealthComponent::takeDamage(Damage dmg) {
    DamageResult result;
    result.inputDamage = dmg.value;

    for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
        if (dmg.value == 0)
            break;

        HealthLayer& layer = *it;
        HealthPoints hpBefore = layer.currentHp;

        Damage out = layer.takeDamage(dmg);

        HealthPoints absorbed = hpBefore - layer.currentHp;
        result.appliedDamage += absorbed;

        result.lastLayerHit = &layer;

        if (layer.isBroken() && hpBefore != 0) {
            if (onLayerBroken != nullptr) onLayerBroken(layer);
            result.layerBroken = true;
        }

        dmg = out;
    }

    result.overflowDamage = dmg.value;
    return result;
}

HealResult HealthComponent::heal(const HealRequest& req) {
    HealResult result;
    result.input = req.value;

    if (req.value == 0)
        return result;

    HealthPoints remaining = req.value;

    auto processLayer = [&](HealthLayer& layer) {
        if (remaining == 0)
            return;

        HealthPoints restored = layer.heal(remaining, req.overhealFactor);

        if (restored > 0) {
            result.restored += restored;
            remaining -= restored;
            result.lastLayer = &layer;
        }
    };

    switch (req.mode) {
        case HealMode::OutsideIn:
            for (auto it = layers.rbegin(); it != layers.rend(); ++it)
                processLayer(*it);
            break;

        case HealMode::InsideOut:
            for (auto& layer : layers)
                processLayer(layer);
            break;

        case HealMode::Specific:
            for (auto& layer : layers) {
                if (layer.id == req.layerId) {
                    processLayer(layer);
                    break;
                }
            }
            break;
    }

    result.overflow = remaining;
    result.fullyHealed = (remaining == 0);
    return result;
}

HealthPoints HealthComponent::repair(StringView id, HealthPoints value, float overheal) {
    for (auto& layer : layers) {
        if (layer.id == id)
            return layer.heal(value, overheal);
    }
    return 0;
}

HealthPoints HealthComponent::regenerate(HealthPoints value, HealthLayerFlags required) {
    HealthPoints remaining = value;
    HealthPoints total = 0;

    for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
        if (remaining == 0)
            break;

        HealthLayer& layer = *it;

        if ((layer.flags & required) != required)
            continue;

        HealthPoints restored = layer.heal(remaining);
        remaining -= restored;
        total += restored;
    }

    return total;
}

HealthPoints HealthComponent::getTotalHp() const {
    return std::accumulate(
        layers.begin(), layers.end(), (HealthPoints) 0,
        [](HealthPoints sum, const HealthLayer& layer) { return sum + layer.currentHp; }
    );
}

HealthPoints HealthComponent::getTotalMaxHp() const {
    return std::accumulate(
        layers.begin(), layers.end(), (HealthPoints) 0,
        [](HealthPoints sum, const HealthLayer& layer) { return sum + layer.maxHealth; }
    );
}

float HealthComponent::getHpRatio() const {
    auto max = getTotalMaxHp();
    return max ? (float) getTotalHp() / max : 0.f;
}

} // namespace cx
