#include "Globals.hpp"

rl::Texture2D TEX_DESERT;
rl::Texture2D TEX_SS_RAT;

void loadTextures() {
    TEX_DESERT = rl::LoadTexture("assets/desert.png");
    TEX_SS_RAT = rl::LoadTexture("assets/Dream Rats/Dream Rat 1.png");
}

unsigned char randChar(int lower, int higher)
{
    return (unsigned char)(rand()%(higher - lower + 1) + lower);
}

int ftint(float x)
{
    if (x > 0)
        return int(x + 0.5f);
    return int(x - 0.5f);
}

int clamp(int num, int lower, int higher)
{
    if (num < lower)
        return lower;
    if (num > higher)
        return higher;
    return num;
}
