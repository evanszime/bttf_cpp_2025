#pragma once

// === ECS CORE ===
#include "Entity.hpp"
#include "CompContainer.hpp"
#include "World.hpp"

// === COMPONENTS ===
#include "Components/Position.hpp"
#include "Components/Health.hpp"
#include "Components/Rigidbody.hpp"
#include "Components/BoxCollider.hpp"
#include "Components/Sprite.hpp"
#include "Components/Animation.hpp"
#include "Components/Input.hpp"
#include "Components/Text.hpp"
#include "Components/SoundTrigger.hpp"
#include "Components/Weapon.hpp"
#include "Components/Player.hpp"
#include "Components/Bullet.hpp"
#include "Components/Wave.hpp"
#include "Components/EnemyAI.hpp"
#include "Components/Health.hpp"
#include "Components/Score.hpp"
#include "Components/Time.hpp"

// === GLOBAL ===
#include "GlobalComponent.hpp"

// === SYSTEMS ===
#include "Systems/System.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/PhysicSystem.hpp"
#include "Systems/HealthSystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/WeaponSystem.hpp"
#include "Systems/EnemySystem.hpp"
#include "Systems/BulletSystem.hpp"
#include "Systems/WaveSystem.hpp"
#include "Systems/SoundSystem.hpp"

// === SCENE ===
#include "SceneManager.hpp"

// === RAYLIB ===
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
