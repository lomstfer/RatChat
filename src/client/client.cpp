#include "Client.hpp"
#include "Menu.hpp"

int main()
{
    rl::SetConfigFlags(rl::FLAG_VSYNC_HINT);
    rl::InitWindow(SCREENW, SCREENH, "");
    rl::SetExitKey(rl::KEY_NULL);
    changeWindowSize(haveMonitorWidth*0.8f, haveMonitorHeight*0.8f);

    loadTextures();

    Menu menu = Menu();
    while (!menu.connect)
    {
        menu.update();
    }

    Client client;
    if (menu.customIP.length() > 0)
        client = Client(menu.customIP.c_str());
    else
        client = Client("192.168.194.68");

    bool in = true;
    while (in)
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
            in = false;
    }
    rl::CloseWindow();
}