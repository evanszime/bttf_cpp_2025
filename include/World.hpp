#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include <algorithm>
#include "Entity.hpp"
#include "CompContainer.hpp"

class World {
    std::size_t nextEntity = 0;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentContainer>> containers;

public:
    bool isClearing = false;

    void clearWorld() {
        isClearing = true;
        for (auto& kv : containers)
            if (kv.second) kv.second->clear();
        nextEntity = 0;
        isClearing = false;
    }

    Entity createEntity() { return Entity(nextEntity++); }

    template<typename Component>
    ComponentContainer<Component>& getContainer() {
        auto id = std::type_index(typeid(Component));
        if (containers.find(id) == containers.end())
            containers[id] = std::make_unique<ComponentContainer<Component>>();
        return *static_cast<ComponentContainer<Component>*>(containers[id].get());
    }

    template<typename Component>
    void addComponent(Entity e, Component c) { getContainer<Component>().add(e, std::move(c)); }

    template<typename Component>
    Component* getComponent(Entity e) { return getContainer<Component>().get(e); }

    template<typename Component>
    void removeComponent(Entity e) { getContainer<Component>().remove(e); }

    template<typename T>
    bool hasContainer() const {
        return containers.find(std::type_index(typeid(T))) != containers.end();
    }

    void copyEntity(Entity src) {
        Entity dest = createEntity();
        for (auto& c : containers) c.second->copy(src, dest);
    }

    void removeEntity(Entity e) {
        for (auto& c : containers) c.second->remove(e);
    }

    std::size_t getNbrEntities() { return nextEntity; }
};
