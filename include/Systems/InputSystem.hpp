#pragma once
#include "System.hpp"
#include <unordered_map>
#include <functional>
#include <raylib.h>

class InputContext {
public:
    // key -> handler(World&, Entity)
    std::unordered_map<int, std::function<void(World&, Entity)>> handlers;
    void bind(int key, std::function<void(World&, Entity)> fn) { handlers[key] = fn; }
    void unbind(int key) { handlers.erase(key); }
    void clear()         { handlers.clear(); }
    bool has(int key)    const { return handlers.count(key) > 0; }
};

class InputSystem : public System {
public:
    InputContext* currentContext = nullptr;
    explicit InputSystem() = default;
    void setInputContext(InputContext* ctx) { currentContext = ctx; }
    void update(float dt) override;
};
