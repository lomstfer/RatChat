#include "Globals.hpp"
#include "GlobalsClient.hpp"
#include <string>

class Menu
{
public:
    Menu()
    {
        rendTarg = rl::LoadRenderTexture(GAMEW, GAMEH);
    }
    ~Menu()
    {
        rl::UnloadRenderTexture(rendTarg);
    }

    bool connect = false;

    rl::RenderTexture2D rendTarg;

    std::string customIP = "";
    rl::Vector2 ipPos = {50,20};
    std::string customPort = "";
    rl::Vector2 portPos = {50,70};

    int connectButtonWidth = 400;
    int connectButtonHeight = 100;
    rl::Rectangle connectButtonRect = {GAMEW/2.f - connectButtonWidth/2, GAMEH/2.f - connectButtonHeight/2, (float)connectButtonWidth, (float)connectButtonHeight};
    rl::Color connectButtonColor = {100,150,100,255};

    enum states {
        STATE_NONE,
        GET_IP,
        GET_PORT
    };

    int state = STATE_NONE;

    void update()
    {
        switchState();
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

            if (rl::IsKeyPressed(rl::KEY_ESCAPE))
                state = STATE_NONE;
            if (rl::IsKeyPressed(rl::KEY_DOWN) || rl::IsKeyPressed(rl::KEY_TAB))
                state = GET_PORT;
            if (rl::IsKeyPressed(rl::KEY_ENTER))
                state = GET_PORT;
            
            render();
            break;

        case GET_PORT:
            getPortInput();

            if (rl::IsKeyPressed(rl::KEY_ESCAPE))
                state = STATE_NONE;
            if (rl::IsKeyPressed(rl::KEY_UP) || rl::IsKeyPressed(rl::KEY_TAB))
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
        rl::BeginTextureMode(rendTarg);
        rl::ClearBackground({20,20,20,255});

        rl::DrawRectangle(connectButtonRect.x, connectButtonRect.y, connectButtonRect.width, connectButtonRect.height, connectButtonColor);
        if (!connect)
        {
            rl::Vector2 connectingTextSize = rl::MeasureTextEx(font, "Connect", 50, 0);
            rl::DrawTextPro(font, "Connect", {GAMEW/2 - connectingTextSize.x/2, GAMEH/2 - connectingTextSize.y/2}, {0,0}, 0, 50, 0, {255,255,255,255});
        }
        if (connect)
        {
            connectRender();
        }
        if (state != STATE_NONE)
        {
            rl::DrawRectangle(10,10,300,150,{40,40,40,255});
            rl::Vector2 textSize = rl::MeasureTextEx(font, customIP.c_str(), 40, 0);
            if (customIP.length() > 0)
                rl::DrawTextPro(font, customIP.c_str(), ipPos, {0,0}, 0, 40, 0, {255,255,255,255});
            else
                rl::DrawTextPro(font, "default ip", ipPos, {0,0}, 0, 40, 0, {100,100,100,255});
            if (state == GET_IP)
            {
                rl::DrawRectangle(ipPos.x + textSize.x, ipPos.y + textSize.y, 25, int(rl::GetTime()*5.0)%2 * 5.f, {255,255,255,255});
                rl::DrawRectangle(ipPos.x - 30, ipPos.y + textSize.y/2, 20, 3, {255,255,255,255});
            }

            textSize = rl::MeasureTextEx(font, customPort.c_str(), 40, 0);
            if (customPort.length())
                rl::DrawTextPro(font, customPort.c_str(), {50, 70}, {0,0}, 0, 40, 0, {255,255,255,255});
            else
                rl::DrawTextPro(font, "default port", {50, 70}, {0,0}, 0, 40, 0, {100,100,100,255});
            if (state == GET_PORT)
            {
                rl::DrawRectangle(portPos.x + textSize.x, portPos.y + textSize.y, 25, int(rl::GetTime()*5.0)%2 * 5.f, {255,255,255,255});
                rl::DrawRectangle(portPos.x - 30, portPos.y + textSize.y/2, 20, 3, {255,255,255,255});
            }
        }
        else {
            rl::DrawTextPro(font, "<esc> | settings", {10, 10}, {0,0}, 0, 25, 0, {255,255,255,255});
            rl::DrawTextPro(font, "<F11> | toggle fullscreen", {10, 40}, {0,0}, 0, 25, 0, {255,255,255,255});
        }
        
        rl::EndTextureMode();

        rl::BeginDrawing();
        rl::ClearBackground({0,0,0,0});
        rl::DrawTexturePro(rendTarg.texture, 
                           {0, 0, (float)rendTarg.texture.width, (float)-rendTarg.texture.height},
                           {0,0,(float)SCREENW,(float)SCREENH}, {0,0}, 0, {255,255,255,255});
        rl::EndDrawing();
    }

    void connectRender()
    {
        rl::Vector2 connectingTextSize = rl::MeasureTextEx(font, "Connecting", 50, 0);
        rl::DrawTextPro(font, "Connecting", {GAMEW/2 - connectingTextSize.x/2, GAMEH/2 - connectingTextSize.y/2}, {0,0}, 0, 50, 0, {255,255,255,255});
        rl::DrawRectangle(GAMEW/2.f + connectingTextSize.x/2 + 5 + 20, GAMEH/2 + connectingTextSize.y - 37, 5, int(rl::GetTime()*1.0)%2 * 5.f, {255,255,255,255});
        rl::DrawRectangle(GAMEW/2.f + connectingTextSize.x/2 + 5 + 10, GAMEH/2 + connectingTextSize.y - 37, 5, int((rl::GetTime()+0.33)*1.0)%2 * 5.f, {255,255,255,255});
        rl::DrawRectangle(GAMEW/2.f + connectingTextSize.x/2 + 5, GAMEH/2 + connectingTextSize.y - 37, 5, int((rl::GetTime()+0.66)*1.0)%2 * 5.f, {255,255,255,255});
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
        if (connect)
            state = STATE_NONE;
        rl::Vector2 mouse_pos = rl::GetMousePosition() * ((float)GAMEW/SCREENW);

        bool hover = false;
        if (rl::CheckCollisionPointRec(mouse_pos, connectButtonRect) && !connect)
        {
            connectButtonColor = {100,150,100,200};
            hover = true;
        }
        else
            connectButtonColor = {100,150,100,255};

        if (rl::IsMouseButtonPressed(rl::MOUSE_BUTTON_LEFT) && hover)
        {
            connect = true;
        }
    }
};