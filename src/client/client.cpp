#include "Client.hpp"

int main()
{
    rl::InitWindow(WINW, WINH, "");
    rl::SetTargetFPS(144);
    Client client("192.168.0.107");

    bool running = true;
    while (running)
    {
        client.update();
        if (rl::WindowShouldClose())
            running = false;
    }
}