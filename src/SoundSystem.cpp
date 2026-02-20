#include "../include/Engine.hpp"

void SoundSystem::update(float dt)
{
    if (!world || !_global) return;

    auto& sounds = world->getContainer<SoundTriggerComponent>();
    for (Entity e : sounds.getEntities()) {
        auto* s = sounds.get(e);
        if (!s || !s->play || s->played) continue;
        if (s->soundIndex >= 0 && s->soundIndex < (int)_global->_allMusics.size()) {
            PlaySound(_global->_allMusics[s->soundIndex]);
            s->played = true;
        }
    }

    // Appliquer volume global
    SetMasterVolume(_global->masterVolume);
}
