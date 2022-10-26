#ifndef SPRITE_SHEET_HPP
#define SPRITE_SHEET_HPP
#include <raylib.h>

// Sprite sheet, with only one row that is

class SpriteSheet {
public:
    SpriteSheet() = default;
    SpriteSheet(rl::Texture2D texture, const int numberOfFrames, float scale, int fps);
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
    int facing;
};
#endif