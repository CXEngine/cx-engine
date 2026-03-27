#pragma once

#include <cx-engine/core/entity/entity.hpp>

#include <cx-engine/utils/bitflags.hpp>
#include <cx-engine/defs/types.hpp>
#include <cx-engine/utils/buf.hpp>
#include <cx-engine/core/entity/component.hpp>

namespace cx {

using HealthPoints = u32;

enum class DamageType {
    Physical,
    Explosion,
    Electrical,
    Chemical,
    Thermal,
};

class HealthLayer;

struct Damage {
    HealthPoints value;
    DamageType type;

    constexpr Damage(HealthPoints v, DamageType t = DamageType::Physical) noexcept
        : value(v), type(t) {}
};

struct DamageResult {
    HealthPoints inputDamage = 0;
    HealthPoints appliedDamage = 0;
    HealthPoints overflowDamage = 0;

    HealthLayer* lastLayerHit = nullptr;
    bool layerBroken = false;
};

enum class HealMode {
    OutsideIn,
    InsideOut,
    Specific,
};

struct HealRequest {
    HealthPoints value = 0;
    HealMode mode = HealMode::OutsideIn;
    StringView layerId = StringView();
    float overhealFactor = 0.f;
};

struct HealResult {
    HealthPoints input = 0;
    HealthPoints restored = 0;
    HealthPoints overflow = 0;

    HealthLayer* lastLayer = nullptr;
    bool fullyHealed = false;
};

enum class HealthLayerFlags {
    None        = 0,
    AllowsChem  = 1 << 0,
    EnergyBased = 1 << 1,

    HumanFlesh = AllowsChem,
    EnergyShield = EnergyBased,
};

TD_BITFLAGS(HealthLayerFlags);

struct HealthAbsorbRule {
    float ratio = 1.0f;
    HealthPoints maxAbsorb = UINT32_MAX;
};

class HealthLayer {
public:
    StringView id;
    HealthLayerFlags flags;
    float overflowFactor = 1.0;

    HealthPoints maxHealth;
    HealthPoints currentHp;
    HealthAbsorbRule absorb;

    HealthLayer(StringView id, HealthPoints maxHealth, HealthLayerFlags flags = HealthLayerFlags::None, float overflowFactor = 1.0)
        : id(id)
        , flags(flags)
        , overflowFactor(overflowFactor)
        , maxHealth(maxHealth)
        , currentHp(maxHealth)
    {}

    HealthLayer(StringView id, HealthPoints maxHealth, HealthPoints currentHp, HealthLayerFlags flags = HealthLayerFlags::None, float overflowFactor = 1.0)
        : id(id)
        , flags(flags)
        , overflowFactor(overflowFactor)
        , maxHealth(maxHealth)
        , currentHp(currentHp)
    {}

    Damage takeDamage(Damage dmg);

    HealthPoints heal(HealthPoints amount, float overhealFactor = 0.f);

    inline bool isBroken() const { return currentHp == 0; }
    inline bool isFull(float overhealFactor = 0.f) const {
        HealthPoints maxAllowed =
            (HealthPoints)(maxHealth * (1.f + overhealFactor));
        return currentHp >= maxAllowed;
    }
};

class HealthComponent: public EntityComponent {
public:
    using LayerBrokenCallback = Func<void(HealthLayer&)>;

protected:
    static constexpr usize MaxHealthLayers = 5;
    Buf<HealthLayer, MaxHealthLayers> layers;

public:
    LayerBrokenCallback onLayerBroken;

    inline HealthComponent(Entity& parent) : EntityComponent(parent), layers() {}
    explicit inline HealthComponent(Entity& parent, InitList<HealthLayer> layers)
        : EntityComponent(parent), layers(layers)
    {}

    template <typename... Args>
    inline void addLayer(Args&&... args) {
        layers.emplace(std::forward<Args>(args)...);
    }

    void update(float) override {
        parent.isAlive = isAlive();
    }

    inline HealthLayer* getCurrentLayer() {
        for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
            if (!it->isBroken()) {
                return &*it;
            }
        }
        return nullptr;
    }

    inline const HealthLayer* getCurrentLayer() const {
        for (auto it = layers.crbegin(); it != layers.crend(); ++it) {
            if (!it->isBroken()) {
                return &*it;
            }
        }
        return nullptr;
    }

    DamageResult takeDamage(Damage dmg);

    HealResult heal(const HealRequest& req);
    HealthPoints repair(StringView id, HealthPoints value, float overheal = 0.f);
    HealthPoints regenerate(HealthPoints value, HealthLayerFlags required);

    inline bool isDead() const { return getCurrentLayer()->currentHp != 0; }
    inline bool isAlive() const { return !isDead(); }

    inline HealthPoints getCurrentLayerHp() const {
        if (auto* l = getCurrentLayer())
            return l->currentHp;
        return 0;
    }

    HealthPoints getTotalHp() const;
    HealthPoints getTotalMaxHp() const;
    float getHpRatio() const;
};

}
