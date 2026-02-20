#include "../include/GlobalComponent.hpp"
#include <iostream>
#include <raylib.h>

GlobalComponent _globalComponent;

void GlobalComponent::addSprite(const std::string& path)
{
    if (FileExists(path.c_str())) {
        Texture2D tex = LoadTexture(path.c_str());
        _allSprites.push_back(tex);
        std::cout << "[Assets] Texture loaded [" << _allSprites.size()-1 << "]: " << path << "\n";
    } else {
        // Si l'asset n'existe pas, on charge une texture placeholder rose 16x16
        Image img = GenImageColor(16, 16, PINK);
        Texture2D tex = LoadTextureFromImage(img);
        UnloadImage(img);
        _allSprites.push_back(tex);
        std::cout << "[Assets] WARNING: " << path << " NOT FOUND - using placeholder\n";
    }
}

void GlobalComponent::addSomeSprites(const std::vector<std::string>& paths)
{
    for (const auto& p : paths)
        addSprite(p);
}

void GlobalComponent::addMusic(const std::string& path)
{
    if (FileExists(path.c_str())) {
        Sound s = LoadSound(path.c_str());
        _allMusics.push_back(s);
        std::cout << "[Assets] Sound loaded [" << _allMusics.size()-1 << "]: " << path << "\n";
    } else {
        std::cout << "[Assets] WARNING: " << path << " NOT FOUND - sound skipped\n";
        // Pas de placeholder son, on pousse un son vide (wave vide)
        Wave w = {};
        Sound s = LoadSoundFromWave(w);
        _allMusics.push_back(s);
    }
}

void GlobalComponent::addSomeMusics(const std::vector<std::string>& paths)
{
    for (const auto& p : paths)
        addMusic(p);
}
