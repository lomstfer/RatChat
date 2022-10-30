#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <string>
#include <raylib.h>
#include <iostream>
#include "SpriteSheet.hpp"
#include "Textures.hpp"

#define Log(x) std::cout << x << std::endl
#define Lognl(x) std::cout << std::endl << x << std::endl
#define WINW 1200
#define WINH 900
#define SPRITE_SCALE 4

struct Player
{
    Player() = default;
    Player(int id, int x, int y, int rat_type, int frame, int rotation)
    : id(id), x(x), y(y), rat_type(rat_type), frame(frame), rotation(rotation) {}
    int id;
    float x;
    float y;
    int rat_type;
    int frame;
    int rotation;
    std::string message;
};

class PlayingCard
{
public:
    PlayingCard(int unique_id, int value, int x, int y)
    : unique_id(unique_id), value(value), x(x), y(y) {}
    int unique_id;
    int value;
    int x;
    int y;
};

enum packetTypes
{
    FLAG_PLAYER_DATA,
    FLAG_PLAYINGCARD_DATA,
    FLAG_PLAYINGCARD_ADD,
    FLAG_PLAYINGCARD_REMOVE,
};

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

float lerp(float v0, float v1, float t) 
{
    return v0 + t * (v1 - v0);
}

std::ostream& operator<<(std::ostream& stream, const rl::Vector2& vec2)
{
    stream << (int)vec2.x << "; " << (int)vec2.y;
    return stream;
}

#endif