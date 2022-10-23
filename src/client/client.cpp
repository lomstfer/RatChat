#include "Client.hpp"

int main()
{
    rl::InitWindow(WINW, WINH, "");
    rl::SetTargetFPS(60);
    Client client("192.168.0.113");

    bool running = true;
    while (running)
    {
        client.update();
        if (rl::WindowShouldClose())
            running = false;
    }
}