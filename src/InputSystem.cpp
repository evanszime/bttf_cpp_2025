#include "../include/Engine.hpp"

void InputSystem::update(float dt) {
    (void)dt;
    if (!world || !currentContext || currentContext->handlers.empty()) return;

    auto handlersCopy = currentContext->handlers;
    std::vector<Entity> entities;
    for (Entity e : world->getContainer<InputComponent>().getEntities())
        entities.push_back(e);

    for (Entity entity : entities) {
        for (auto& [key, fn] : handlersCopy) {
            if (IsKeyDown(key))
                fn(*world, entity);
        }
    }
}
