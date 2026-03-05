#include "../include/Engine.hpp"
void SoundSystem::update(float dt) {
    (void)dt;
    if (!world || !_global) return;
    auto& triggers = world->getContainer<SoundTriggerComponent>();
    for (Entity e : triggers.getEntities()) {
        auto* t = triggers.get(e); if (!t) continue;
        for (auto& [name, sd] : t->sounds) {
            if (!sd.isActive) continue;
            if ((int)_global->_allMusics.size() <= sd.soundPath) continue;
            SetSoundVolume(_global->_allMusics[sd.soundPath], sd.volume * _global->masterVolume);
            PlaySound(_global->_allMusics[sd.soundPath]);
            sd.countPlay--;
            if (!sd.loop && sd.countPlay <= 0) sd.isActive = false;
        }
    }
}
