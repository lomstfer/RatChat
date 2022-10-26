#include "Client.hpp"

int main()
{
    rl::InitWindow(WINW, WINH, "");
    rl::SetTargetFPS(rl::GetMonitorRefreshRate(rl::GetCurrentMonitor()));
    loadTextures();
    Client client("188.150.137.71");

    bool running = true;
    while (running)
    {
        client.update();
        if (rl::WindowShouldClose())
            running = false;
    }
}