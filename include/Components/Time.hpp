#pragma once

struct TimeComponent {
    float deltaTime {0.f};
    float totalTime {0.f};
    float fixedDelta {1.f/60.f};
    float accumulator {0.f};
    float scale {1.f};
    bool paused {false};
};
