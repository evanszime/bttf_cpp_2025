#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "../Entity.hpp"

/**
 * Enhanced EnemyAI Component for R-Type
 * Supports all classic R-Type enemy patterns and behaviors
 */
struct EnemyAIComponent {
    // === EXISTING PROPERTIES (kept for compatibility) ===
    int behavior = 0;               // Behavior ID
    std::string scriptName;         // Lua script name
    bool isenemy = true;           // Is this an enemy entity
    float a = 50.0f;               // Generic parameter A (amplitude)
    float b = 1.0f;                // Generic parameter B (frequency)
    
    // === MOVEMENT PARAMETERS ===
    float speed = 100.0f;          // Base movement speed
    float maxSpeed = 200.0f;       // Maximum allowed speed
    float acceleration = 500.0f;   // Acceleration rate
    float turnSpeed = 3.0f;        // Rotation speed (radians/sec)
    
    // === PATTERN PARAMETERS ===
    float amplitude = 50.0f;       // Pattern amplitude (sine, circle radius)
    float frequency = 1.0f;        // Pattern frequency
    float phase = 0.0f;            // Current phase in pattern
    float angle = 0.0f;            // Current angle (for rotational patterns)
    
    // === COMBAT PARAMETERS ===
    float detectionRange = 400.0f; // Range to detect players
    float attackRange = 300.0f;    // Range to start attacking
    float fireRate = 1.0f;         // Shots per second
    float lastFireTime = 0.0f;     // Time since last shot
    float bulletSpeed = 300.0f;    // Speed of projectiles
    int burstCount = 1;            // Bullets per burst
    float spreadAngle = 0.0f;      // Bullet spread angle
    
    // === TARGET TRACKING ===
    Entity targetEntity = Entity(0);
    float targetX = 0.0f;          // Target/Center position
    float targetY = 0.0f;
    bool hasTarget = false;
    
    // === AI STATE MACHINE ===
    enum AIState {
        IDLE = 0,
        SPAWNING = 1,
        ACTIVE = 2,
        ATTACKING = 3,
        FLEEING = 4,
        DYING = 5
    };
    AIState state = SPAWNING;
    float stateTimer = 0.0f;
    
    // === WAYPOINT SYSTEM ===
    std::vector<std::pair<float, float>> waypoints;
    int currentWaypoint = 0;
    bool loopWaypoints = true;
    float waypointThreshold = 20.0f;
    
    // === FORMATION DATA ===
    Entity leader = Entity(0);
    float offsetX = 0.0f;          // Formation offset
    float offsetY = 0.0f;
    int formationIndex = 0;        // Position in formation
    
    // === CUSTOM LUA DATA ===
    std::unordered_map<std::string, float> customData;
    
    // === SPECIAL PARAMETERS ===
    float spiralRadius = 10.0f;    // For spiral patterns
    float orbitRadius = 100.0f;    // For circular patterns
    int bossPhase = 0;             // Boss phase
    bool isInvulnerable = false;   // Temporary invulnerability
    
    // === HISTORY FOR SNAKE PATTERN ===
    std::pair<float, float> lastPosition;
    float lastAngle = 0.0f;
    std::vector<std::pair<float, float>> positionHistory;
    size_t historySize = 20;
    int followDelay = 5;
    std::string bullet_type;
};