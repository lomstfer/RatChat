#include "Globals.hpp"
#include "GlobalsClient.hpp"
#include <string>

class Menu
{
public:
    Menu()
    {
    }

    bool connect = false;

    std::string customIP = "";
    rl::Vector2 ipPos = {50,20};
    std::string customPort = "25565";
    rl::Vector2 portPos = {50,70};

    int connectButtonWidth = 400;
    int connectButtonHeight = 100;
    rl::Rectangle connectButtonRect = {GAMEW/2 - connectButtonWidth/2, GAMEH/2 - connectButtonHeight/2, connectButtonWidth, connectButtonHeight};

    enum states {
        STATE_NONE,
        GET_IP,
        GET_PORT
    };

    int state = STATE_NONE;

    void update()
    {
        switch (state)
        {
        case STATE_NONE:
            if (rl::IsKeyPressed(rl::KEY_ESCAPE))
                state = GET_IP;
            if (rl::IsKeyPressed(rl::KEY_ENTER))
                connect = true;

            render();
            break;

        case GET_IP:
            getIPInput();

            if (rl::IsKeyPressed(rl::KEY_DOWN))
                state = GET_PORT;
            if (rl::IsKeyPressed(rl::KEY_ENTER))
                state = GET_PORT;

            render();
            break;

        case GET_PORT:
            getPortInput();

            if (rl::IsKeyPressed(rl::KEY_UP))
                state = GET_IP;
            if (rl::IsKeyPressed(rl::KEY_ENTER))
                state = STATE_NONE;

            render();
            break;
        
        default:
            break;
        }
        
        if (rl::WindowShouldClose())
            rl::CloseWindow();
    }

    void render()
    {
        rl::BeginDrawing();
        rl::ClearBackground({20,20,20,255});

        rl::DrawRectangle(connectButtonRect.x, connectButtonRect.y, connectButtonRect.width, connectButtonRect.height, {100,150,100,255});

        rl::Vector2 textSize = rl::MeasureTextEx(font, customIP.c_str(), 40, 0);
        rl::DrawTextPro(font, customIP.c_str(), ipPos, {0,0}, 0, 40, 0, {255,255,255,255});
        if (state == GET_IP)
        {
            rl::DrawRectangle(ipPos.x + textSize.x, ipPos.y + textSize.y, 25, int(rl::GetTime()*5.0)%2 * 5.f, {255,255,255,255});
            rl::DrawRectangle(ipPos.x - 30, ipPos.y + textSize.y/2, 20, 3, {255,255,255,255});
        }

        textSize = rl::MeasureTextEx(font, customPort.c_str(), 40, 0);
        rl::DrawTextPro(font, customPort.c_str(), {50, 70}, {0,0}, 0, 40, 0, {255,255,255,255});
        if (state == GET_PORT)
        {
            rl::DrawRectangle(portPos.x + textSize.x, portPos.y + textSize.y, 25, int(rl::GetTime()*5.0)%2 * 5.f, {255,255,255,255});
            rl::DrawRectangle(portPos.x - 30, portPos.y + textSize.y/2, 20, 3, {255,255,255,255});
        }

        rl::EndDrawing();
    }

    void getIPInput()
    {
        // Get char pressed (unicode character) on the queue
        int key = rl::GetCharPressed();

        while (key > 0)
        {
            if ((key >= 48 && key <= 57) || key == 46)
            {
                customIP += char(key);
            }
            key = rl::GetCharPressed();  // Check next character in the queue
        }

        if (rl::IsKeyPressed(rl::KEY_BACKSPACE) && customIP.length() > 0)
        {
            customIP.pop_back();
        }
    }

    void getPortInput()
    {
        // Get char pressed (unicode character) on the queue
        int key = rl::GetCharPressed();

        while (key > 0)
        {
            if (key >= 48 && key <= 57)
            {
                customPort += char(key);
            }
            key = rl::GetCharPressed();  // Check next character in the queue
        }

        if (rl::IsKeyPressed(rl::KEY_BACKSPACE) && customPort.length() > 0)
        {
            customPort.pop_back();
        }
    }

    void switchState()
    {
        rl::Vector2 mouse_pos = rl::GetMousePosition() * ((float)GAMEW/SCREENW);

        /* if (rl::IsMouseButtonPressed(rl::MOUSE_BUTTON_LEFT) &&
            rl::CheckCollisionPointRec(mouse_pos, {})) */
    }

};

