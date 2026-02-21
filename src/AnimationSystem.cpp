#include "../include/Engine.hpp"
void AnimationSystem::update(float dt) {
    if (!world || !_global) return;
    auto& anims   = world->getContainer<AnimationComponent>();
    auto& sprites = world->getContainer<SpriteComponent>();
    std::vector<Entity> toDestroy;
    for (Entity e : anims.getEntities()) {
        auto* anim = anims.get(e); auto* spr = sprites.get(e);
        if (!anim || !spr || !anim->playing) continue;
        auto it1 = _global->_allAnimations.find(anim->animations);
        if (it1 == _global->_allAnimations.end()) continue;
        auto it2 = it1->second.find(anim->currentAnim);
        if (it2 == it1->second.end()) continue;
        auto& seq = it2->second;
        if (seq.frames.empty()) continue;
        anim->elapsed += dt * anim->speed;
        if (anim->elapsed >= seq.fps) {
            anim->elapsed = 0.f;
            if (seq.loop) {
                anim->currentFrame = (anim->currentFrame+1) % seq.frames.size();
            } else {
                if (anim->currentFrame+1 < seq.frames.size()) anim->currentFrame++;
                else { anim->playing = false; if (anim->autoDestroy) { toDestroy.push_back(e); continue; } }
            }
        }
        const auto& frame = seq.frames[anim->currentFrame];
        spr->left=frame.rect.x; spr->top=frame.rect.y;
        spr->width=frame.rect.width; spr->height=frame.rect.height;
    }
    for (Entity e : toDestroy) world->removeEntity(e);
}
