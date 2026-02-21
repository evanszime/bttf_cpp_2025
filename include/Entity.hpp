#pragma once
#include <functional>
#include <cstddef>

class Entity {
public:
    Entity() = default;
    Entity(std::size_t id) : id_(id) {}

    operator std::size_t() const { return id_; }

    bool operator==(const Entity& other) const noexcept {
        return id_ == other.id_;
    }

private:
    std::size_t id_;
};

namespace std {
    template<>
    struct hash<Entity> {
        std::size_t operator()(const Entity& e) const noexcept {
            return std::hash<std::size_t>()(static_cast<std::size_t>(e));
        }
    };
}
