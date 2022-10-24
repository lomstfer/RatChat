#include "Client.hpp"

int main()
{
    rl::InitWindow(WINW, WINH, "");
    rl::SetTargetFPS(144);
    Client client("192.168.56.1");

    bool running = true;
    while (running)
    {
        client.update();
        if (rl::WindowShouldClose())
            running = false;
    }
}