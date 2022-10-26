#include "Client.hpp"

int main()
{
    rl::InitWindow(WINW, WINH, "");
    rl::SetTargetFPS(60);
    loadTextures();
    Client client("192.168.194.79");

    bool running = true;
    while (running)
    {
        client.update();
        if (rl::WindowShouldClose())
            running = false;
    }
}