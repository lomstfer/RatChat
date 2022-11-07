#include "Client.hpp"
#include "Menu.hpp"

#define DEFAULT_IP "192.168.194.68"
#define DEFAULT_PORT 25565

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
    client = Client(menu.customIP.length() == 0 ? DEFAULT_IP : menu.customIP.c_str(), 
                    menu.customPort.length() == 0 ? DEFAULT_PORT : std::stoi(menu.customPort));

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