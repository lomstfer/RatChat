#include "Client.hpp"
#include "Menu.hpp"

int main()
{
    rl::SetConfigFlags(rl::FLAG_VSYNC_HINT);
    rl::InitWindow(SCREENW, SCREENH, "Ratchat");
    rl::SetWindowIcon(rl::LoadImageFromTexture(rl::LoadTexture("assets/drat_0.png")));
    rl::SetExitKey(rl::KEY_NULL);
    changeWindowSize(haveMonitorWidth*0.8f, haveMonitorHeight*0.8f);

    loadTextures();

    while (true)
    {
        // menu
        MENU:
        Menu menu = Menu();
        while (true)
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
            menu.update();
            if (menu.connect)
                break;
            if (rl::WindowShouldClose())
                goto EXIT;
        }

        Client client(menu.customIP.length() == 0 ? DEFAULT_IP : menu.customIP.c_str(), 
                        menu.customPort.length() == 0 ? DEFAULT_PORT : std::stoi(menu.customPort));
        
        // connecting
        while (!client.isConnected)
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

            client.connectUpdate();
            menu.update();

            if (rl::IsKeyPressed(rl::KEY_ESCAPE))
                goto MENU;
            if (rl::WindowShouldClose())
                goto EXIT;
        }
        
        // in the world
        while (true)
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
            if (!client.isConnected)
                break;
            if (rl::IsKeyPressed(rl::KEY_ESCAPE))
                break;
            if (rl::WindowShouldClose())
                goto EXIT;
        }
        if (rl::WindowShouldClose())
            break;
    }
    EXIT:
    rl::CloseWindow();
}