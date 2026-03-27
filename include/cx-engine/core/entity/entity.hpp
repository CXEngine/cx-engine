#pragma once

#include <SFML/Window/Event.hpp>

#include <cx-engine/core/obj/sprite-object.hpp>

#include <cx-engine/core/entity/component.hpp>
#include <cx-engine/utils/hybrid-ptr.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <type_traits>
#include <typeindex>

namespace cx {

class Team;
class Entity: public SpriteObject {
protected:
    HashMap<TypeIndex, HybridPtr<EntityComponent>> components;

public:
    bool isAlive = true;

public:
    Entity(SpriteObject base);
    Entity(const sf::Texture& texture, const sf::IntRect& rect);

    void update(float dt);
    void handle(const sf::Event& event);
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;

    template <typename TComponent>
    requires std::is_base_of_v<EntityComponent, TComponent>
    TComponent& addComponent(HybridPtr<TComponent> component) {
        components[typeid(TComponent)] = std::move(component);
        return static_cast<TComponent&>(*components[typeid(TComponent)].get());
    }

    template <typename TComponent, typename... Args>
    requires std::is_base_of_v<EntityComponent, TComponent>
    TComponent& addComponent(Args&&... args) {
        auto component = HybridPtr<TComponent>::Make(std::forward<Args>(args)...);
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
