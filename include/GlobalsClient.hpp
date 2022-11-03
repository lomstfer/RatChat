#ifndef GLOBALS_CLIENT_HPP
#define GLOBALS_CLIENT_HPP

#include <iostream>
#include <raylib.h>

std::ostream& operator<<(std::ostream& stream, const rl::Vector2& vec2);

rl::Vector2 operator*(rl::Vector2 vec2, const float multiplier);

void changeWindowSize(int newWidth, int newHeight);

extern int SCREENW;
extern int SCREENH;

#define RAT_WIDTH 24
#define RAT_HEIGHT 24

extern rl::Texture2D TEX_DESERT;

extern rl::Texture2D TEX_SS_RAT0;
extern rl::Texture2D TEX_SS_RAT1;
extern rl::Texture2D TEX_SS_RAT2;
extern rl::Texture2D TEX_SS_RAT3;
extern rl::Texture2D TEX_SS_RAT4;
extern rl::Texture2D TEX_SS_RAT5;
extern rl::Texture2D TEX_SS_RATDREAM;
extern rl::Texture2D TEX_RATS[7];

extern rl::Texture2D TEX_CARDS_SHEET;
extern rl::Texture2D TEX_CARDS[14];

void loadTextures();

#endif