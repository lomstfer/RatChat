#include "Client.hpp"

int main()
{
    rl::SetConfigFlags(rl::FLAG_VSYNC_HINT);
    rl::InitWindow(WINW, WINH, "");
    rl::SetTargetFPS(60);
    loadTextures();
    Client client("192.168.0.113");

    bool running = true;
    while (running)
    {
        client.update();
        if (rl::WindowShouldClose())
            running = false;
    }
    rl::CloseWindow();
}