#include "../include/Engine.hpp"

void AnimationSystem::update(float dt)
{
    if (!world || !_global) return;

    auto& anims   = world->getContainer<AnimationComponent>();
    auto& sprites = world->getContainer<SpriteComponent>();

    std::vector<Entity> toDestroy;

    for (Entity e : anims.getEntities()) {
        auto* anim = anims.get(e);
        auto* spr  = sprites.get(e);
        if (!anim || !spr || !anim->playing) continue;

        // Chercher le jeu d'animations
        auto setIt = _global->_allAnimations.find(anim->animations);
        if (setIt == _global->_allAnimations.end()) continue;

        auto animIt = setIt->second.find(anim->currentAnim);
        if (animIt == setIt->second.end()) continue;

        AnimationSequence& seq = animIt->second;
        if (seq.frames.empty()) continue;

        if (anim->currentFrame >= seq.frames.size())
            anim->currentFrame = 0;

        float speed = (anim->speed > 0.f) ? anim->speed : 1.f;
        anim->elapsed += dt * speed;

        float frameDur = (seq.fps > 0.f) ? (1.f / seq.fps) : 0.125f;

        if (anim->elapsed >= frameDur) {
            anim->elapsed = 0.f;
            if (seq.loop) {
                anim->currentFrame = (anim->currentFrame + 1) % seq.frames.size();
            } else {
                if (anim->currentFrame + 1 < seq.frames.size()) {
                    anim->currentFrame++;
                } else {
                    anim->playing = false;
                    if (anim->autoDestroy) {
                        toDestroy.push_back(e);
                        continue;
                    }
                }
            }
        }

        const AnimationFrame& frame = seq.frames[anim->currentFrame];
        spr->left   = (float)frame.rect.x;
        spr->top    = (float)frame.rect.y;
        spr->width  = (float)frame.rect.width;
        spr->height = (float)frame.rect.height;
    }

    for (Entity e : toDestroy)
        world->removeEntity(e);
}
