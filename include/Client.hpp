#include "game_state_generated.h"
#include "Globals.hpp"
#include "SpriteSheet.hpp"
#include <raylib.h>
#include <iostream>
#include <enet/enet.h>
#include <string>
#include <vector>
#include <math.h>

struct Client
{
    Client(const char* ip_address)
    {
        if (enet_initialize() != 0)
            Log("error: init enet");
        atexit(enet_deinitialize);
        client = enet_host_create(NULL, 1, 1, 0, 0);
        if (client == NULL)
            Log("error: create client");
        enet_address_set_host(&address, ip_address);
        address.port = 25565;
        host = enet_host_connect(client, &address, 1, 0);
        if (host == NULL)
            Log("error: connecting to host");
        fb_builder = flatbuffers::FlatBufferBuilder(1024);
        font = rl::LoadFont("assets/UbuntuCondensed-Regular.ttf");
        bgTexture = TEX_DESERT;
        bgSize = {(float)bgTexture.width*bgScale, (float)bgTexture.height*bgScale};
        for (int x = -2; x < 2; x++)
        {
            for (int y = -2; y < 2; y++)
            {
                bg_list.push_back({x*bgSize.x, y*bgSize.y});
            }
        }
        ratSheet = SpriteSheet(TEX_SS_RAT, 8, 8, 12);
        _w = ratSheet.frameWidth * ratSheet.scale;
        _h = ratSheet.frameHeight * ratSheet.scale;
        _camera_x = -WINW/2.f + _w/2.f;
        _camera_y = -WINH/2.f + _h/2.f;
    }

    ENetHost* client;
    ENetAddress address;
    ENetEvent event;
    ENetPeer* host;

    flatbuffers::FlatBufferBuilder fb_builder;

    rl::Font font;
    rl::Texture2D bgTexture;
    float bgScale = 5;
    rl::Vector2 bgSize;

    int send_fps = 10;
    float send_time = 0;
    bool send_now = false;

    rl::Color colorBg = {20, 20, 20, 255};
    std::vector<rl::Vector2> bg_list;

    // players vector with server positions
    std::vector<Player> players_server;
    // players vector here that are getting smoothed out
    std::vector<Player> players_show;
    std::vector<Player> temp_players;

    SpriteSheet ratSheet;
    float _x = 0;
    float _y = 0;
    int _w;
    int _h;
    int _scale = 5;
    bool _pressed = false;
    float _pressed_time = 0;
    int _speed = 200;

    float _camera_x;
    float _camera_y;

    int _id;

    std::string message = "";
    std::string typeMessage = "";
    bool typingMessage = false;

    float dt;
    void update()
    {
        dt = rl::GetFrameTime();
        send_now = false;
        if (enet_host_service(client, &event, 0) > 0) 
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    _id = event.peer->outgoingPeerID;
                    send_now = true;
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    // get world info (all players data)
                    auto game_state = GS::GetGameState(event.packet->data);
                    players_server.clear();
                    for (int i = 0; i < game_state->players()->size(); i++)
                    {
                        players_server.emplace_back(game_state->players()->Get(i)->id(), game_state->players()->Get(i)->x(), game_state->players()->Get(i)->y());
                        players_server.back().message = flatbuffers::GetString(game_state->players()->Get(i)->message());
                    }
                    temp_players = players_show;
                    players_show = players_server;
                    for (int i = 0; i < temp_players.size(); i++)
                    {
                        if (players_show[i].id == temp_players[i].id)
                        {
                            players_show[i].x = temp_players[i].x;
                            players_show[i].y = temp_players[i].y;
                        }
                    }
                    enet_packet_destroy(event.packet);
                    break;
            }
        }

        getMessageSend();

        inputSend();

        // interpolate other players positions to their new positions that come from server and are server-known
        interpolatePlayers();

        render();
    }

    void getMessageSend()
    {
        if (rl::IsKeyPressed(rl::KEY_ENTER) && typingMessage)
        {
            message = typeMessage;
            fb_builder.Clear();
            fb_builder.Finish(GS::CreatePlayer(fb_builder, _id, _x, _y, fb_builder.CreateString(message)));
            enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize()+1, 0));
            typeMessage.clear();
        }
        if (rl::IsKeyPressed(rl::KEY_ENTER))
            typingMessage = !typingMessage;
        if (typingMessage)
        {
            // Get char pressed (unicode character) on the queue
            int key = rl::GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {
                typeMessage += char(key);
                key = rl::GetCharPressed();  // Check next character in the queue
            }

            if (rl::IsKeyPressed(rl::KEY_BACKSPACE) && typeMessage.length() > 0)
                typeMessage.pop_back();
        }
        
    }

    void inputSend()
    {
        _pressed_time += dt;
        if (_pressed_time >= 1)
        {
            _pressed_time = 0;
            _pressed = false;
        }
        if (rl::IsKeyDown(rl::KEY_LEFT))
        {
            _pressed = true;
            _x -= _speed * dt;
        }
        if (rl::IsKeyDown(rl::KEY_RIGHT))
        {
            _pressed = true;
            _x += _speed * dt;
        }
        if (rl::IsKeyDown(rl::KEY_UP))
        {
            _pressed = true;
            _y -= _speed * dt;
        }
        if (rl::IsKeyDown(rl::KEY_DOWN))
        {
            _pressed = true;
            _y += _speed * dt;
        }

        send_time += dt;
        if ((send_time >= 1.f/send_fps && _pressed == true) || send_now)
        {
            send_time = 0;
            fb_builder.Clear();
            fb_builder.Finish(GS::CreatePlayer(fb_builder, _id, _x, _y));
            enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
        }
        

        float d_cam_x = _x - _camera_x - WINW/2.f + _w/2.f;
        float d_cam_y = _y - _camera_y - WINH/2.f + _h/2.f;
        _camera_x += d_cam_x*5.f * dt;
        _camera_y += d_cam_y*5.f * dt;
    }

    void interpolatePlayers()
    {
        for (int i = 0; i < players_show.size(); i++)
        {
            if (players_show[i].id == players_server[i].id)
            {
            if (players_show[i].x < players_server[i].x)
            {
                players_show[i].x += _speed * dt;
                if (players_show[i].x > players_server[i].x)
                    players_show[i].x = players_server[i].x;
            }
            if (players_show[i].x > players_server[i].x)
            {
                players_show[i].x -= _speed * dt;
                if (players_show[i].x < players_server[i].x)
                    players_show[i].x = players_server[i].x;
            }
            if (players_show[i].y < players_server[i].y)
            {
                players_show[i].y += _speed * dt;
                if (players_show[i].y > players_server[i].y)
                    players_show[i].y = players_server[i].y;
            }
            if (players_show[i].y > players_server[i].y)
            {
                players_show[i].y -= _speed * dt;
                if (players_show[i].y < players_server[i].y)
                    players_show[i].y = players_server[i].y;
            }
            }
        }
    }

    void fixDrawBackground()
    {
        for (int i = 0; i < bg_list.size(); i++)
        {
            float rowcol = sqrt(bg_list.size());
            if (_x - bg_list[i].x < -bgSize.x)
                bg_list[i].x -= rowcol*bgSize.x;
            if (_x - bg_list[i].x > bgSize.x*(rowcol/2) + bgSize.x/2)
                bg_list[i].x += rowcol*bgSize.x;
            if (_y - bg_list[i].y < -bgSize.y)
                bg_list[i].y -= rowcol*bgSize.y;
            if (_y - bg_list[i].y > bgSize.y*(rowcol/2) + bgSize.y/2)
                bg_list[i].y += rowcol*bgSize.y;
            
            rl::DrawTextureEx(bgTexture, {bg_list[i].x - _camera_x, bg_list[i].y - _camera_y}, 0, bgScale, {255,255,255,255});
        }
    }

    void render()
    {
        rl::BeginDrawing();
            rl::ClearBackground(colorBg);
            fixDrawBackground();

            for (int i = 0; i < players_show.size(); i++)
            {
                rl::Vector2 msgTextSize = rl::MeasureTextEx(font, players_show[i].message.c_str(), 20, 0);
                if (players_show[i].id == _id)
                {
                    rl::Vector2 pDrawPos = {_x - _camera_x, _y - _camera_y};
                    //rl::DrawRectangle(pDrawPos.x - _w/2.f, pDrawPos.y - _h/2.f, _w, _h, {255, 255, 255, 255});
                    //rl::DrawTextureEx(ratTexture, {pDrawPos.x - _w/2.f, pDrawPos.y - _h/2.f}, 0, _scale, {255,255,255,255});
                    ratSheet.draw({pDrawPos.x, pDrawPos.x}, 0);
                    rl::DrawTextPro(font, players_show[i].message.c_str(), {pDrawPos.x - msgTextSize.x/2, pDrawPos.y - 40}, {0,0}, 0, 20, 0, {255,255,255,255});
                    continue;
                }
                rl::Vector2 pDrawPos = {players_show[i].x - _camera_x, players_show[i].y - _camera_y};
                //rl::DrawRectangle(pDrawPos.x - _w/2.f, pDrawPos.y - _h/2.f, _w, _h, {255,255,255,255});
                ratSheet.draw({pDrawPos.x, pDrawPos.x}, 0);
                rl::DrawTextPro(font, players_show[i].message.c_str(), {pDrawPos.x - msgTextSize.x/2, pDrawPos.y - 40}, {0,0}, 0, 20, 0, {255,255,255,255});
            }

            std::string coordsText = std::to_string(ftint(_x/10.f)) + "; " + std::to_string(ftint(-_y/10.f));
            rl::DrawTextPro(font, coordsText.c_str(), {0,0}, {0,0}, 0, 20, 0, {255,255,255,255});

            if (typingMessage)
            {
                rl::Vector2 msgTextSize = rl::MeasureTextEx(font, typeMessage.c_str(), 50, 0);
                rl::DrawTextPro(font, typeMessage.c_str(), {WINW/2.f - msgTextSize.x/2, 50}, {0,0}, 0, 50, 0, {255,255,255,255});
                rl::DrawRectangle(WINW/2.f + msgTextSize.x/2, 50 + msgTextSize.y - 5, 25, (int)rl::GetTime()%2 * 5.f, {255,255,255,200});
            }
            
        rl::EndDrawing();
    }
};