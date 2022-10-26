#include "SpriteSheet.hpp"

SpriteSheet::SpriteSheet(rl::Texture2D texture, const int numberOfFrames, float scale, int fps)
: texture(texture), numberOfFrames(numberOfFrames), scale(scale), fps(fps) {
    frameWidth = texture.width / numberOfFrames;
    frameHeight = texture.height;
    currentFrame = 0;
    facing = 1;
    srcRect = {float(frameWidth * currentFrame), 0.0f, (float)frameWidth * facing, (float)texture.height};
    dstRect = {0, 0, (float)frameWidth * scale, (float)texture.height * scale};
}

void SpriteSheet::animate(float deltaTime) {
    timer += deltaTime;
    if (timer >= 1.0f / fps) {
        timer = 0.0f;
        currentFrame += 1;
    }
    currentFrame = currentFrame % numberOfFrames;
    srcRect = {float(frameWidth * currentFrame), 0.0f, (float)frameWidth * facing, (float)texture.height};
}

void SpriteSheet::draw(rl::Vector2 drawPosition, float rotation) {
    dstRect = {drawPosition.x, drawPosition.y, (float)frameWidth * scale, (float)texture.height * scale};
    rl::DrawTexturePro(texture, srcRect, dstRect, {frameWidth/2 * scale, frameHeight/2 * scale}, rotation, {255, 255, 255, 255});
}
