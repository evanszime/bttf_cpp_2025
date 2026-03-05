#include "../include/GlobalComponent.hpp"
#include <iostream>

GlobalComponent _globalComponent;

// Couleurs fallback
static Color FALLBACK_COLORS[] = {
    {0,200,255,255},{255,60,60,255},{200,0,0,255},{255,255,0,255},
    {255,120,0,255},{100,255,100,255},{100,200,255,255},{200,200,100,255},
    {255,100,0,255},{0,255,0,255},{20,20,40,255},{255,220,0,255},
};
static int FALLBACK_SIZE=12;
static int FALLBACK_W[]={32,32,64,8,8,24,24,24,24,12,1920,200};
static int FALLBACK_H[]={32,32,64,8,8,24,24,24,24,12,1080,80};

void GlobalComponent::addSprite(const std::string& path) {
    int idx=(int)_allSprites.size();
    if (FileExists(path.c_str())) {
        _allSprites.push_back(LoadTexture(path.c_str()));
        std::cout<<"[Assets] Texture ["<<idx<<"] OK: "<<path<<"\n";
    } else {
        int w=(idx<FALLBACK_SIZE)?FALLBACK_W[idx]:32;
        int h=(idx<FALLBACK_SIZE)?FALLBACK_H[idx]:32;
        Color col=(idx<FALLBACK_SIZE)?FALLBACK_COLORS[idx]:MAGENTA;
        Image img=GenImageColor(w,h,col);
        _allSprites.push_back(LoadTextureFromImage(img));
        UnloadImage(img);
        std::cout<<"[Assets] Texture ["<<idx<<"] MISSING -> placeholder: "<<path<<"\n";
    }
}

void GlobalComponent::addSomeSprites(const std::vector<std::string>& paths) {
    for (auto& p : paths) addSprite(p);
}

void GlobalComponent::addMusic(const std::string& path) {
    int idx=(int)_allMusics.size();
    if (FileExists(path.c_str())) {
        _allMusics.push_back(LoadSound(path.c_str()));
        std::cout<<"[Assets] Sound ["<<idx<<"] OK: "<<path<<"\n";
    } else {
        Wave w={}; w.frameCount=1; w.sampleRate=44100; w.sampleSize=16; w.channels=1;
        short silence=0; w.data=&silence;
        _allMusics.push_back(LoadSoundFromWave(w));
        std::cout<<"[Assets] Sound ["<<idx<<"] MISSING -> silence: "<<path<<"\n";
    }
}

void GlobalComponent::addSomeMusics(const std::vector<std::string>& paths) {
    for (auto& p : paths) addMusic(p);
}
