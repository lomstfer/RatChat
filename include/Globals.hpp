#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <string>
#include <raylib.h>

#define Log(x) std::cout << x << std::endl
#define Lognl(x) std::cout << std::endl << x << std::endl
#define WINW 1280
#define WINH 720
#define SPRITE_SCALE 4

struct Player
{
    Player() = default;
    Player(int id, int x, int y)
    : id(id), x(x), y(y) {}
    int id;
    float x;
    float y;
    std::string message;
};

unsigned char randChar(int lower, int higher);
int ftint(float x);
int clamp(int num, int lower, int higher);

extern rl::Texture2D TEX_DESERT;
extern rl::Texture2D TEX_SS_RAT;
void loadTextures();

#endif