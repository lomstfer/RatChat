#ifndef SPRITE_SHEET_HPP
#define SPRITE_SHEET_HPP
#include <raylib.h>

void drawSheetFrame(rl::Texture2D& texture, int frame, int frameWidth, int frameHeight, rl::Vector2 drawPosition, float rotation, float scale, rl::Vector2 origin);

class SpriteSheet {
public:
    SpriteSheet() = default;
    SpriteSheet(rl::Texture2D texture, int numberOfFrames, float scale, int fps, rl::Vector2 origin);
    void animate(float deltaTime);
    void draw(rl::Vector2 drawPosition, float rotation);

    rl::Texture2D texture;
    int numberOfFrames; 
    int frameWidth;
    int frameHeight;
    float scale;
    
    int currentFrame = 0;
    float timer = 0;
    int fps;

    rl::Rectangle srcRect;
    rl::Rectangle dstRect;

    rl::Vector2 origin;
};



#endif