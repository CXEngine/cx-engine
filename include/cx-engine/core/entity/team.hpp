#pragma once

#include <cx-engine/core/entity/entity.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/Graphics/Color.hpp>

namespace cx {

class Entity;

class Team {
protected:
    Set<Entity*> members;

public:
    sf::Color color;
    // String name;

public:
    void addMember(Entity* entity) {
        if (entity == nullptr) return;
        members.insert(entity);
    }

    void delMember(Entity* entity) {
        if (entity == nullptr) return;
        members.erase(entity);
    }

    bool includes(Entity* entity) const {
        if (entity == nullptr) return false;
        return members.contains(entity);
    }

    usize getMembersCount() const {
        return members.size();
    }

    auto begin() { return members.begin(); }
    auto end() { return members.end(); }

    auto begin() const { return members.begin(); }
    auto end() const { return members.end(); }

    auto cbegin() const { return members.cbegin(); }
    auto cend() const { return members.cend(); }

    auto rbegin() { return members.rbegin(); }
    auto rend() { return members.rend(); }

    auto rbegin() const { return members.rbegin(); }
    auto rend() const { return members.rend(); }

    auto crbegin() const { return members.crbegin(); }
    auto crend() const { return members.crend(); }
};

}
