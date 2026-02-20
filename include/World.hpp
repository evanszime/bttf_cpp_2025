#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <algorithm> 
#include "Entity.hpp"
#include "CompContainer.hpp"
#include "Components/Sprite.hpp"

class World {
    std::size_t nextEntity = 0;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentContainer>> containers;
    
public:
    bool isClearing = false;
    // ...
    void clearWorld() {
        isClearing = true;
        // appeler clear() sur chaque container (au lieu de supprimer les unique_ptr immédiatement)
        for (auto &kv : containers) {
            if (kv.second) kv.second->clear();
        }
        // maintenant on peut effacer la map
        // containers.clear();
        // ne réinitialise PAS nextEntity si tu veux éviter d'avoir des id réutilisés immédiatement
        nextEntity = 0; // -> évite réutiliser les IDs (optionnel selon design)
        isClearing = false;
    }
    Entity createEntity() {
        return Entity(nextEntity++);
    }

    template<typename Component>
    ComponentContainer<Component>& getContainer() {
        auto id = std::type_index(typeid(Component));
        // std::cout << "dde\n";
        if (containers.find(id) == containers.end()) {
            containers[id] = std::make_unique<ComponentContainer<Component>>();
        }
        // std::cout << "final\n";
        return *static_cast<ComponentContainer<Component>*>(containers[id].get());
    }

    template<typename Component>
    void addComponent(Entity e, Component c) {
        getContainer<Component>().add(e, std::move(c));
    }

    template<typename Component>
    Component* getComponent(Entity e) {
        return getContainer<Component>().get(e);
    }

    template<typename Component>
    void removeComponent(Entity e) {
        getContainer<Component>().remove(e);
    }
    void copyEntity(Entity src) {
        Entity dest = createEntity();
        for (auto& container : containers) {
            container.second->copy(src, dest);
        }
    }

    std::size_t getNbrEntities() {
        return nextEntity;
    }

    void removeEntity(Entity e) {
        for (auto& container : containers) {
            container.second->remove(e);
        }
    }
    template<typename T>
    bool hasContainer() const {
        return containers.find(std::type_index(typeid(T))) != containers.end();
    }

    std::vector<Entity> getEntitiesByTag(int tag) {
        std::vector<Entity> result;

        // 🔹 Récupérer les containers qui peuvent avoir un tag
        auto& sprites = getContainer<SpriteComponent>();
        // auto& circles = getContainer<CircleComponent>();
        // auto& rectangles = getContainer<RectangleComponent>();

        // 🔹 Vérifier chaque type de composant
        for (Entity e : sprites.getEntities()) {
            if (auto* s = sprites.get(e)) {
                if (s->tag == tag)
                    result.push_back(e);
            }
        }

        // for (Entity e : circles.getEntities()) {
        //     if (auto* c = circles.get(e)) {
        //         if (c->tag == tag)
        //             result.push_back(e);
        //     }
        // }

        // for (Entity e : rectangles.getEntities()) {
        //     if (auto* r = rectangles.get(e)) {
        //         if (r->tag == tag)
        //             result.push_back(e);
        //     }
        // }

        return result;
    }
};