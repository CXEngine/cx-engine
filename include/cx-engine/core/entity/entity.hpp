#pragma once

#include <SFML/Window/Event.hpp>

#include <cx-engine/core/entity/object.hpp>

#include <cx-engine/core/entity/component.hpp>
#include <cx-engine/utils/hybrid-ptr.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace cx {

class SpriteComponent;

class Team;
class Entity: public Object {
protected:
    HashMap<TypeIndex, HybridPtr<EntityComponent>> components;

public:
    bool isAlive = true;

public:
    Entity() = default;
    Entity(Object base);
    Entity(const sf::Texture& texture, const sf::IntRect& rect);

    void update(float dt) override;
    void handle(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override;

    virtual void onPositionUpdate() override;

    virtual void setPosition(const sf::Vector2f& position);
    virtual void setRotation(sf::Angle angle);
    virtual void setScale(const sf::Vector2f& factors);
    virtual void setOrigin(const sf::Vector2f& origin);

    SpriteComponent& sprite();
    const SpriteComponent& sprite() const;

    template <typename TComponent>
    requires std::is_base_of_v<EntityComponent, TComponent>
    TComponent& addComponent(HybridPtr<TComponent> component) {
        components[typeid(TComponent)] = std::move(component);
        return static_cast<TComponent&>(*components[typeid(TComponent)].get());
    }

    template <typename TComponent, typename... Args>
    requires std::is_base_of_v<EntityComponent, TComponent>
    TComponent& addComponent(Args&&... args) {
        auto component = HybridPtr<TComponent>::Make(*this, std::forward<Args>(args)...);
        return addComponent(std::move(component));
    }

    template <typename TComponent>
    requires std::is_base_of_v<EntityComponent, TComponent>
    TComponent* getComponent() {
        auto it = components.find(typeid(TComponent));
        if (it != components.end())
            return static_cast<TComponent*>(it->second.get());
        return nullptr;
    }

    template <typename TComponent>
    requires std::is_base_of_v<EntityComponent, TComponent>
    const TComponent* getComponent() const {
        auto it = components.find(typeid(TComponent));
        if (it != components.end())
            return static_cast<const TComponent*>(it->second.get());
        return nullptr;
    }

    template <typename TComponent>
    requires std::is_base_of_v<EntityComponent, TComponent>
    void enableComponent() {
        if (TComponent* comp = getComponent<TComponent>()) {
            comp->setEnabled(true);
        }
    }

    template <typename TComponent>
    requires std::is_base_of_v<EntityComponent, TComponent>
    void disableComponent() {
        if (TComponent* comp = getComponent<TComponent>()) {
            comp->setEnabled(false);
        }
    }
};

} // namespace cx
