#include "SpriteSheet.hpp"

void drawSheetFrame(rl::Texture2D& texture, int frame, int frameWidth, int frameHeight, rl::Vector2 drawPosition, float rotation, float scale, rl::Vector2 origin)
{
    rl::DrawTexturePro(texture, {float(frameWidth * frame), 0.0f, (float)frameWidth, (float)texture.height}, {drawPosition.x, drawPosition.y, (float)frameWidth * scale, (float)frameHeight * scale}, {origin.x * scale, origin.y * scale}, rotation, {255, 255, 255, 255});
}

SpriteSheet::SpriteSheet(rl::Texture2D texture, int numberOfFrames, float scale, int fps, rl::Vector2 origin)
: texture(texture), numberOfFrames(numberOfFrames), scale(scale), fps(fps), origin(origin) {
    frameWidth = texture.width / numberOfFrames;
    frameHeight = texture.height;
    currentFrame = 0;
    srcRect = {float(frameWidth * currentFrame), 0.0f, (float)frameWidth, (float)texture.height};
    dstRect = {0, 0, (float)frameWidth * scale, (float)texture.height * scale};
}

void SpriteSheet::animate(float deltaTime) {
    timer += deltaTime;
    if (timer >= 1.0f / fps) {
        timer = 0.0f;
        currentFrame += 1;
    }
    currentFrame = currentFrame % numberOfFrames;
    srcRect = {float(frameWidth * currentFrame), 0.0f, (float)frameWidth, (float)texture.height};
}

void SpriteSheet::draw(rl::Vector2 drawPosition, float rotation) {
    dstRect = {drawPosition.x, drawPosition.y, (float)frameWidth * scale, (float)frameHeight * scale};
    rl::DrawTexturePro(texture, srcRect, dstRect, {origin.x * scale, origin.y * scale}, rotation, {255, 255, 255, 255});
}