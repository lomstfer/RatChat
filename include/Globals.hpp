#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <chrono>
#include <thread>

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

#endif