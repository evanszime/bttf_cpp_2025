#pragma once
#include <vector>
#include <unordered_map>
#include "Entity.hpp"
#include <iostream>

class IComponentContainer {
public:
    virtual ~IComponentContainer() = default;
    virtual void copy(Entity, Entity) = 0;
    virtual void remove(Entity e) = 0;
    virtual void clear() = 0;
};

template<typename Component>
class ComponentContainer: public IComponentContainer
{ 
    std::vector<Component> components; 
    std::vector<Entity> entities; 
    std::unordered_map<Entity, size_t> indexOf; // entity -> index in vec
public: 
    void add(Entity e, Component c) { 
        if (indexOf.find(e) != indexOf.end()) return; // déjà présent 
        indexOf[e] = components.size(); 
        components.push_back(std::move(c)); 
        entities.push_back(e); 
    }
    void clear() override {
        components.clear();
        entities.clear();
        indexOf.clear();
    }
    bool has(Entity e) const { 
        return indexOf.find(e) != indexOf.end(); 
    }
    Component* get(Entity e) { 
        auto it = indexOf.find(e); 
        if (it == indexOf.end()) return nullptr; 
        return &components[it->second]; 
    }
    void remove(Entity e) override { 
        auto it = indexOf.find(e); 
        if (it == indexOf.end()) return;
        size_t idx = it->second; 
        size_t last = components.size() - 1; 
        // swap with le dernier élément 
        components[idx] = std::move(components[last]); 
        entities[idx] = entities[last]; 
        // mettre à jour l'index de l'entité qui a été déplacée 
        indexOf[entities[idx]] = idx; 
        // enlever le dernier 
        components.pop_back(); 
        entities.pop_back(); 
        indexOf.erase(it); 
    }
    // itération rapide sur tous les components
    void copy(Entity src, Entity dest) override {
        auto it = indexOf.find(src);
        if (it == indexOf.end()) return;
        size_t idx = it->second;
        Component cpy = components[idx];
        add(dest, std::move(cpy));
    }
    std::vector<Component> getComponents() const { return components; }
    std::vector<Entity> getEntities() const { return entities; }
    std::vector<Component> getComponents() { return components; }
    std::vector<Entity> getEntities() { return entities; }
    std::unordered_map<Entity, size_t> getIndexMap() const { return indexOf; }
    auto begin() { return components.begin(); } 
    auto end()   { return components.end(); } 
};