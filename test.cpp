#include <iostream>
#include <raylib.h>

#define p(x) std::cout << x << std::endl

int main()
{
    rl::InitWindow(640,360,"");
    bool pressedLastFrame = false;
    while (true)
    {
        if (rl::IsKeyPressed(rl::KEY_L) && pressedLastFrame)
            p("pressed");
        //pressedLastFrame = false;
        if (rl::IsKeyPressed(rl::KEY_L))
            pressedLastFrame = true;

        if (rl::WindowShouldClose())
            break;

        rl::BeginDrawing();
        rl::ClearBackground({0,0,0,0});
        rl::EndDrawing();
    }
    rl::CloseWindow();
}