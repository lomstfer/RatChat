#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <string>
#include <iostream>
#include <time.h>

#define Log(x) std::cout << x << std::endl
#define Lognl(x) std::cout << std::endl << x << std::endl

#define DEFAULT_IP "192.168.194.68"
#define DEFAULT_PORT 25565

#define GAMEW 1600
#define GAMEH 900
#define SPRITE_SCALE 4

#define card_dims_x 25.f * SPRITE_SCALE
#define card_dims_y 35.f * SPRITE_SCALE

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
    PlayingCard() = default; 
    PlayingCard(int unique_id, int value, int color, int x, int y, bool flipped)
    : unique_id(unique_id), value(value), color(color), x(x), y(y), flipped(flipped) {}
    int unique_id;
    int value;
    int color;
    int x;
    int y;
    bool flipped;
};

enum packetTypes
{
    FLAG_PLAYER_DATA,
    FLAG_PLAYINGCARD_DATA,
    FLAG_PLAYINGCARD_ADD,
    FLAG_PLAYINGCARD_REMOVE,
    FLAG_PLAYINGCARD_UPDATE,
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

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

#endif