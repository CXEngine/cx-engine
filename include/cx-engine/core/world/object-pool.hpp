#pragma once

#include <cx-engine/utils/dynarena.hpp>

#include <cx-engine/defs/types.hpp>
#include <cx-engine/core/obj/sprite-object.hpp>

#include <cx-engine/gameplay/weapons/bullet.hpp>
#include <cx-engine/core/entity/entity.hpp>
#include <cx-engine/systems/collision.hpp>
#include <typeinfo>

namespace cx {

class ObjectPool {
protected:
    DynArena arena;
    Vec<Object*> objects;

public:
    ObjectPool() = default;

    template <typename TObject, typename... Args>
    requires std::is_base_of_v<Object, TObject>
    TObject& create(Args&&... args) {
        objects.push_back(
            arena.create<TObject>(std::forward<Args>(args)...)
        );
        return *static_cast<TObject*>(objects.back());
    }

    void update(float dt) {
        for (usize i = 0; i < objects.size(); ) {
            objects[i]->update(dt);

            if (objects[i]->isAlive) {
                if (cx::Bullet* bullet = dynamic_cast<cx::Bullet*>(objects[i])) {
                    for (usize j = 0; j < objects.size(); ++j) {
                        if (i == j) continue;

                        if (cx::Entity* entity = dynamic_cast<cx::Entity*>(objects[j])) {
                            if (cx::intersects(bullet->getCollisionPolygon(), entity->getCollisionPolygon())) {
                                BulletHitInfo hitInfo{*entity, bullet->getPosition()};
                                bullet->onHit(hitInfo);
                            }
                        }
                    }
                }
            }

            if (!objects[i]->isAlive) {
                objects[i] = objects.back();
                objects.pop_back();
            } else {
                ++i;
            }
        }
    }

    void handle(const sf::Event& event) {
        for (const auto& obj: objects) {
            obj->handle(event);
        }
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const {
        for (const auto& obj: objects) {
            target.draw(*obj);
        }
    }
};

}
