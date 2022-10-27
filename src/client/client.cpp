#include "Client.hpp"

int main()
{
    rl::InitWindow(WINW, WINH, "");
    rl::SetTargetFPS(/* rl::GetMonitorRefreshRate(rl::GetCurrentMonitor()) */60);
    rl::SetWindowState(rl::FLAG_FULLSCREEN_MODE);
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