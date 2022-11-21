#include "GlobalsClient.hpp"

int SCREENW = 0;
int SCREENH = 0;

float TEXT_SIZE = 1;

std::ostream& operator<<(std::ostream& stream, const rl::Vector2& vec2)
{
    stream << (int)vec2.x << "; " << (int)vec2.y;
    return stream;
}

rl::Vector2 operator*(rl::Vector2 vec2, const float multiplier)
{
    return {vec2.x * multiplier, vec2.y * multiplier};
}

void changeWindowSize(int width, int height) {
    rl::SetWindowPosition((haveMonitorWidth-width)/2,(haveMonitorHeight-height)/2);
    rl::SetWindowSize(width, height);
    setScreenVars(width, height);
}

void setScreenVars(int width, int height)
{
    SCREENW = width;
    SCREENH = height;
    if (rl::IsWindowMaximized() && !rl::IsWindowFullscreen())
    {
        SCREENW = rl::GetMonitorWidth(rl::GetCurrentMonitor());
        SCREENH = rl::GetMonitorHeight(rl::GetCurrentMonitor());
    }
}

rl::Font font;
rl::Font font2;

rl::Texture2D TEX_DESERT;

rl::Texture2D TEX_SS_RAT;
rl::Texture2D TEX_SS_RAT0;
rl::Texture2D TEX_SS_RAT1;
rl::Texture2D TEX_SS_RAT2;
rl::Texture2D TEX_SS_RAT3;
rl::Texture2D TEX_SS_RAT4;
rl::Texture2D TEX_SS_RAT5;
rl::Texture2D TEX_SS_RATDREAM;
rl::Texture2D TEX_RATS[7];

rl::Texture2D TEX_CARDS[14];
rl::Texture2D TEX_CARDS_SHEET;

void loadTextures() {
    font = rl::LoadFontEx("assets/Roboto-Regular.ttf", 128, 0, 250);
    font2 = rl::LoadFontEx("assets/NanumPenScript-Regular.ttf", 128, 0, 250);

    TEX_DESERT = rl::LoadTexture("assets/desert.png");
    TEX_SS_RAT0 = rl::LoadTexture("assets/Rats/Dream Rat 0.png");
    TEX_SS_RAT1 = rl::LoadTexture("assets/Rats/Dream Rat 1.png");
    TEX_SS_RAT2 = rl::LoadTexture("assets/Rats/Dream Rat 2.png");
    TEX_SS_RAT3 = rl::LoadTexture("assets/Rats/Dream Rat 3.png");
    TEX_SS_RAT4 = rl::LoadTexture("assets/Rats/Dream Rat 4.png");
    TEX_SS_RAT5 = rl::LoadTexture("assets/Rats/Dream Rat 5.png");
    TEX_SS_RATDREAM = rl::LoadTexture("assets/Rats/Dream Rat Rare.png");
    TEX_RATS[0] = TEX_SS_RAT0;
    TEX_RATS[1] = TEX_SS_RAT1;
    TEX_RATS[2] = TEX_SS_RAT2;
    TEX_RATS[3] = TEX_SS_RAT3;
    TEX_RATS[4] = TEX_SS_RAT4;
    TEX_RATS[5] = TEX_SS_RAT5;
    TEX_RATS[6] = TEX_SS_RATDREAM;

    TEX_CARDS_SHEET = rl::LoadTexture("assets/card_deck.png");
}
