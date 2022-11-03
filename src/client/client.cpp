#include "Client.hpp"

int main()
{
    rl::SetConfigFlags(rl::FLAG_VSYNC_HINT);
    rl::InitWindow(WINW, WINH, "");
    int width = rl::GetMonitorWidth(rl::GetCurrentMonitor());
    int height = rl::GetMonitorHeight(rl::GetCurrentMonitor());
    changeWindowSize(width, height);
    rl::ToggleFullscreen();

    loadTextures();
    Log(width);
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