#include "../include/GlobalComponent.hpp"
#include <iostream>

GlobalComponent _globalComponent;

// Couleurs fallback par index (utilisées si le fichier n'existe pas)
static Color FALLBACK_COLORS[] = {
    {0,   200, 255, 255}, // [0]  player      - cyan
    {255,  60,  60, 255}, // [1]  enemy       - rouge
    {200,   0,   0, 255}, // [2]  boss        - rouge foncé
    {255, 255,   0, 255}, // [3]  bullet_pl   - jaune
    {255, 120,   0, 255}, // [4]  bullet_en   - orange
    {100, 255, 100, 255}, // [5]  pickup_p    - vert clair
    {100, 200, 255, 255}, // [6]  pickup_s    - bleu clair
    {200, 200, 100, 255}, // [7]  pickup_sn   - kaki
    {255, 100,   0, 255}, // [8]  pickup_r    - orange foncé
    {  0, 255,   0, 255}, // [9]  xp_orb      - vert vif
    { 20,  20,  40, 255}, // [10] background  - bleu nuit
    {255, 220,   0, 255}, // [11] logo        - or
};
static const int FALLBACK_SIZE = 12;

// Tailles fallback (w, h) pour chaque index
static int FALLBACK_W[] = {32, 32, 64, 8, 8, 24, 24, 24, 24, 12, 1920, 200};
static int FALLBACK_H[] = {32, 32, 64, 8, 8, 24, 24, 24, 24, 12, 1080, 80};

void GlobalComponent::addSprite(const std::string& path) {
    int idx = (int)_allSprites.size();
    if (FileExists(path.c_str())) {
        _allSprites.push_back(LoadTexture(path.c_str()));
        std::cout << "[Assets] Texture [" << idx << "] OK: " << path << "\n";
    } else {
        // Placeholder: carré de la couleur correspondante
        int w = (idx < FALLBACK_SIZE) ? FALLBACK_W[idx] : 32;
        int h = (idx < FALLBACK_SIZE) ? FALLBACK_H[idx] : 32;
        Color col = (idx < FALLBACK_SIZE) ? FALLBACK_COLORS[idx] : MAGENTA;
        Image img = GenImageColor(w, h, col);
        _allSprites.push_back(LoadTextureFromImage(img));
        UnloadImage(img);
        std::cout << "[Assets] Texture [" << idx << "] MANQUANT -> placeholder couleur: " << path << "\n";
    }
}

void GlobalComponent::addSomeSprites(const std::vector<std::string>& paths) {
    for (auto& p : paths) addSprite(p);
}

void GlobalComponent::addMusic(const std::string& path) {
    int idx = (int)_allMusics.size();
    if (FileExists(path.c_str())) {
        _allMusics.push_back(LoadSound(path.c_str()));
        std::cout << "[Assets] Sound  [" << idx << "] OK: " << path << "\n";
    } else {
        Wave w = {};
        _allMusics.push_back(LoadSoundFromWave(w));
        std::cout << "[Assets] Sound  [" << idx << "] MANQUANT -> silence: " << path << "\n";
    }
}

void GlobalComponent::addSomeMusics(const std::vector<std::string>& paths) {
    for (auto& p : paths) addMusic(p);
}
