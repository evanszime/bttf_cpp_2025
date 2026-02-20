#pragma once

struct SoundTriggerComponent {
    int  soundIndex = -1;  // index dans GlobalComponent::_allMusics
    bool play       = false;
    bool loop       = false;
    bool played     = false;
};
