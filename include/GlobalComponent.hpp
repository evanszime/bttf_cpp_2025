#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <raylib.h>
#include "Components/Animation.hpp"

class GlobalComponent {
public:
    // TEXTURES
    std::vector<Texture2D> _allSprites;

    // ANIMATIONS (utilisées par AnimationSystem)
    std::unordered_map<std::string,
        std::unordered_map<std::string, AnimationSequence>> _allAnimations;

    // SONS (Sound = court) + MUSIQUE (Music = longue, streamée)
    std::vector<Sound> _allMusics;   // SFX courts + sons UI
    Music              bgMusic = {};  // musique de fond (streamée)
    bool               bgMusicLoaded = false;

    // SETTINGS
    float masterVolume  = 0.8f;
    float musicVolume   = 0.7f;
    float sfxVolume     = 1.0f;
    float brightness    = 1.0f;   // 0.2 .. 1.0
    bool  isFullscreen  = false;

    // Helpers
    void addSprite(const std::string& path);
    void addSomeSprites(const std::vector<std::string>& paths);
    void addMusic(const std::string& path);
    void addSomeMusics(const std::vector<std::string>& paths);

    // Jouer un SFX par index (volume = sfxVolume * masterVolume)
    void playSFX(int idx, float vol = 1.0f) {
        if(idx < 0 || idx >= (int)_allMusics.size()) return;
        SetSoundVolume(_allMusics[idx], vol * sfxVolume * masterVolume);
        PlaySound(_allMusics[idx]);
    }

    // Changer la musique de fond
    void playBGMusic(const std::string& path) {
        if(bgMusicLoaded) { StopMusicStream(bgMusic); UnloadMusicStream(bgMusic); }
        bgMusic = LoadMusicStream(path.c_str());
        bgMusicLoaded = true;
        SetMusicVolume(bgMusic, musicVolume * masterVolume);
        PlayMusicStream(bgMusic);
    }

    void updateBGMusic() {
        if(bgMusicLoaded) {
            SetMusicVolume(bgMusic, musicVolume * masterVolume);
            UpdateMusicStream(bgMusic);
        }
    }

    void stopBGMusic() {
        if(bgMusicLoaded) StopMusicStream(bgMusic);
    }

    // Luminosité système via xrandr (Linux) — fallback silencieux si indispo
    void applySystemBrightness() {
        // xrandr --output <monitor> --brightness <val>
        // On essaie d'abord de récupérer le nom du moniteur actif
        char cmd[256];
        snprintf(cmd, sizeof(cmd),
            "xrandr | grep ' connected' | head -1 | awk '{print $1}' > /tmp/zimex_mon.txt");
        if (system(cmd) == 0) {
            FILE* f = fopen("/tmp/zimex_mon.txt","r");
            if (f) {
                char mon[64] = {};
                if (fgets(mon,sizeof(mon),f)) {
                    // Enlever le newline
                    for(int i=0;mon[i];i++) if(mon[i]=='\n'||mon[i]=='\r') mon[i]=0;
                    if (mon[0]) {
                        char cmd2[256];
                        snprintf(cmd2,sizeof(cmd2),
                            "xrandr --output %s --brightness %.2f 2>/dev/null",
                            mon, (double)brightness);
                        system(cmd2);
                    }
                }
                fclose(f);
            }
        }
        // Le fallback overlay est géré dans chaque scène via brightness
    }
};

extern GlobalComponent _globalComponent;
