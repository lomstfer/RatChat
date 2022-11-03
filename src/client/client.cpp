#include "Client.hpp"

int main()
{
    rl::SetConfigFlags(rl::FLAG_VSYNC_HINT);
    rl::InitWindow(SCREENW, SCREENH, "");
    changeWindowSize(haveMonitorWidth*0.8f, haveMonitorHeight*0.8f);

    loadTextures();
    Client client("192.168.0.113");

    bool running = true;
    while (running)
    {
        if (rl::IsKeyPressed(rl::KEY_F11))
        {
            bool wasFullscreen = rl::IsWindowFullscreen();
            if (!wasFullscreen)
                changeWindowSize(haveMonitorWidth, haveMonitorHeight);
            rl::ToggleFullscreen();
            if (wasFullscreen)
                changeWindowSize(haveMonitorWidth*0.8f, haveMonitorHeight*0.8f);
        }
        
        client.update();
        if (rl::WindowShouldClose())
            running = false;
    }
    rl::CloseWindow();
}