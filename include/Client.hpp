#include "game_state_generated.h"
#include "GlobalsClient.hpp"
#include "SpriteSheet.hpp"
#include "Textures.hpp"
#include <iostream>
#include <enet/enet.h>
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
        font2 = rl::LoadFont("assets/NanumPenScript-Regular.ttf");

        bgTexture = TEX_DESERT;
        bgSize = {(float)bgTexture.width*SPRITE_SCALE, (float)bgTexture.height*SPRITE_SCALE};
        for (int x = -3; x < 3; x++)
        {
            for (int y = -3; y < 3; y++)
            {
                bg_list.push_back({x*bgSize.x, y*bgSize.y});
            }
        }

        _ratType = rand()%7;
        ratSheet = SpriteSheet(TEX_RATS[_ratType], 8, SPRITE_SCALE, 12, {12,7});
        _camera_x = -WINW/2.f + ratSheet.frameWidth/2.f;
        _camera_y = -WINH/2.f + ratSheet.frameHeight/2.f;

        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 13; i++)
            {
                cards_on_hand.emplace_back(rand()%999999, i+1, j, 0, 0, true);
            }
        }
    }

    ENetHost* client;
    ENetAddress address;
    ENetEvent event;
    ENetPeer* host;

    flatbuffers::FlatBufferBuilder fb_builder;

    rl::Font font;
    rl::Font font2;
    rl::Texture2D bgTexture;
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

    std::vector<PlayingCard> cards_on_ground;
    std::vector<PlayingCard> cards_on_hand;
    rl::Vector2 card_dims_hand = {card_dims_x * 1.5f, card_dims_y * 1.5f};
    rl::Vector2 card_dims_hover = {card_dims_x * 1.7f, card_dims_y * 1.7f};
    rl::Vector2 card_dims_drag = {card_dims_x * 1.3f, card_dims_y * 1.3f};
    float cards_on_hand_space = card_dims_hand.x/1.5f;
    int cards_id_increment = 0;
    PlayingCard card_moving;
    bool moving_card = false;
    bool from_hand = false;
    bool from_ground = false;

    SpriteSheet ratSheet;
    int _ratType;

    float _x = 0;
    float _y = 0;
    float _rotation = 0;

    bool _pressed = false;
    float _pressed_time = 0;
    int _speed = 200;
    float _speed_x = 0;
    float _speed_y = 0;

    float _camera_x;
    float _camera_y;

    int _id;

    std::string message = "";
    std::string typeMessage = "";
    bool isTypingMessage = false;

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
                    getGameStateInfo(event.packet->data);
                    enet_packet_destroy(event.packet);
                    break;
            }
        }

        getMessageSend();

        inputSend();

        cardsInputSend();

        cardsManage();

        // interpolate other players positions to their new positions that come from server and are server-known
        interpolatePlayers();

        render();
    }

    void getGameStateInfo(const void* buf)
    {
        auto game_state = GS::GetGameState(event.packet->data);
        
        players_server.clear();
        for (int i = 0; i < game_state->players()->size(); i++)
        {
            auto player = game_state->players()->Get(i);
            players_server.emplace_back(player->id(), player->x(), player->y(), player->rat_type(), player->frame(), player->rotation());
            players_server.back().message = flatbuffers::GetString(player->message());
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

        cards_on_ground.clear();
        for (int i = 0; i < game_state->cards_on_ground()->size(); i++)
        {
            auto card = game_state->cards_on_ground()->Get(i);
            cards_on_ground.emplace_back(card->unique_id(), card->value(), card->color(), card->x(), card->y(), card->flipped());
        }
    }

    void getMessageSend()
    {
        if (rl::IsKeyPressed(rl::KEY_ENTER) && isTypingMessage)
        {
            message = typeMessage;
            fb_builder.Clear();
            fb_builder.Finish(GS::CreatePlayer(fb_builder, FLAG_PLAYER_DATA, _id, _x, _y, _ratType, ratSheet.currentFrame, int(_rotation), fb_builder.CreateString(message)));
            enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize()+1, 0));
            typeMessage.clear();
        }
        if (rl::IsKeyPressed(rl::KEY_ENTER))
            isTypingMessage = !isTypingMessage;
        if (isTypingMessage)
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
        _speed_x = 0;
        _speed_y = 0;
        _pressed_time += dt;
        if (_pressed_time >= 1)
        {
            _pressed_time = 0;
            _pressed = false;
        }
        if (rl::IsKeyDown(rl::KEY_LEFT))
        {
            _pressed = true;
            _speed_x = -_speed;
        }
        if (rl::IsKeyDown(rl::KEY_RIGHT))
        {
            _pressed = true;
            _speed_x = _speed;
        }
        if (rl::IsKeyDown(rl::KEY_UP))
        {
            _pressed = true;
            _speed_y = -_speed;
        }
        if (rl::IsKeyDown(rl::KEY_DOWN))
        {
            _pressed = true;
            _speed_y = _speed;
        }
        if (_speed_x || _speed_y)
        {
            ratSheet.animate(dt);
            _rotation = atan2(_speed_y, _speed_x) * 180.f/PI + 90;
        }

        _x += _speed_x * dt;
        _y += _speed_y * dt;
        send_time += dt;
        if ((send_time >= 1.f/send_fps && _pressed == true) || send_now)
        {
            send_time = 0;
            fb_builder.Clear();
            fb_builder.Finish(GS::CreatePlayer(fb_builder, FLAG_PLAYER_DATA, _id, _x, _y, _ratType, ratSheet.currentFrame, int(_rotation)));
            enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
        }

        float d_cam_x = _x - _camera_x - WINW/2.f + ratSheet.frameWidth/2.f;
        float d_cam_y = _y - _camera_y - WINH/2.f + ratSheet.frameHeight/2.f;
        _camera_x += d_cam_x*5.f * dt;
        _camera_y += d_cam_y*5.f * dt;
    }

    void addCardSend(const PlayingCard& card)
    {
        cards_on_ground.emplace_back(rand()%999999, card.value, card.color, card.x, card.y, card.flipped);
        fb_builder.Clear();
        fb_builder.Finish(GS::CreatePlayingCard(fb_builder, FLAG_PLAYINGCARD_DATA, FLAG_PLAYINGCARD_ADD, cards_on_ground.back().unique_id, cards_on_ground.back().value, cards_on_ground.back().color, cards_on_ground.back().x, cards_on_ground.back().y, cards_on_ground.back().flipped));
        enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
    }

    void removeCardSend(int unique_id)
    {
        fb_builder.Clear();
        fb_builder.Finish(GS::CreatePlayingCard(fb_builder, FLAG_PLAYINGCARD_DATA, FLAG_PLAYINGCARD_REMOVE, unique_id));
        enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
    }

    void updateCardSend(const PlayingCard& card)
    {
        fb_builder.Clear();
        fb_builder.Finish(GS::CreatePlayingCard(fb_builder, FLAG_PLAYINGCARD_DATA, FLAG_PLAYINGCARD_UPDATE, card.unique_id, card.value, card.color, card.x, card.y, card.flipped));
        enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
    }

    void cardsManage()
    {
        if (rl::IsMouseButtonPressed(rl::MOUSE_BUTTON_LEFT))
        {
            for (int i = cards_on_hand.size() - 1; i >= 0; i--)
            {
                if (rl::CheckCollisionPointRec(rl::GetMousePosition(), {(float)cards_on_hand[i].x, (float)cards_on_hand[i].y, card_dims_hand.x, card_dims_hand.y}))
                {
                    card_moving = cards_on_hand[i];
                    moving_card = true;
                    cards_on_hand.erase(cards_on_hand.begin() + i);
                    from_hand = true;
                    return;
                }
            }

            for (int i = cards_on_ground.size() - 1; i >= 0; i--)
            {
                if (rl::CheckCollisionPointRec(rl::GetMousePosition(), {(float)cards_on_ground[i].x - _camera_x, (float)cards_on_ground[i].y - _camera_y, card_dims_x, card_dims_y}) 
                    /* && rl::CheckCollisionRecs({_x-4*SPRITE_SCALE, _y-4*SPRITE_SCALE, (float)ratSheet.frameWidth+2*SPRITE_SCALE, (float)ratSheet.frameHeight+2*SPRITE_SCALE}, {(float)cards_on_ground[i].x, (float)cards_on_ground[i].y, card_dims_x, card_dims_y}) */)
                {
                    Log("pressedcardonground");
                    card_moving = cards_on_ground[i];
                    removeCardSend(card_moving.unique_id);
                    cards_on_ground.erase(cards_on_ground.begin() + i);
                    moving_card = true;
                    from_ground = true;
                    return;
                }
            }
        }

        if (rl::IsMouseButtonReleased(rl::MOUSE_BUTTON_LEFT) && moving_card)
        {
            moving_card = false;
            if (rl::GetMousePosition().y > WINH-card_dims_hand.y-50)
            {
                from_ground = false;
                from_hand = false;
                cards_on_hand.push_back(card_moving);
            }
            else if (rl::GetMousePosition().y <= WINH-card_dims_hand.y-50)
            {
                card_moving.x = rl::GetMousePosition().x + _camera_x - card_dims_x/2;
                card_moving.y = rl::GetMousePosition().y + _camera_y - card_dims_y/2;
                if (from_ground) {
                    from_ground = false;
                    cards_on_ground.push_back(card_moving);
                    addCardSend(card_moving);
                }
                if (from_hand) {
                    from_hand = false;
                    addCardSend(card_moving);
                }
            }
        }
    }

    void cardsInputSend()
    {
        if (isTypingMessage)
            return;
        if (rl::IsKeyDown(rl::KEY_LEFT_SHIFT))
        {
            if (rl::IsKeyPressed(rl::KEY_SPACE))
            {
                for (int i = cards_on_ground.size() - 1; i >= 0; i--)
                {
                    if (rl::CheckCollisionRecs({_x-4*SPRITE_SCALE, _y-4*SPRITE_SCALE, (float)ratSheet.frameWidth+2*SPRITE_SCALE, (float)ratSheet.frameHeight+2*SPRITE_SCALE}, {(float)cards_on_ground[i].x, (float)cards_on_ground[i].y, card_dims_x, card_dims_y}))
                    {
                        cards_on_hand.push_back(cards_on_ground[i]);
                        fb_builder.Clear();
                        auto card = GS::CreatePlayingCard(fb_builder, FLAG_PLAYINGCARD_DATA, FLAG_PLAYINGCARD_REMOVE, cards_on_ground[i].unique_id);
                        fb_builder.Finish(card);
                        enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
                        break;
                    }
                }
            }

            int temp_value = -1;

            if (rl::IsKeyPressed(rl::KEY_ONE))
                temp_value = 1;
            if (rl::IsKeyPressed(rl::KEY_TWO))
                temp_value = 2;
            if (rl::IsKeyPressed(rl::KEY_THREE))
                temp_value = 3;
            if (rl::IsKeyPressed(rl::KEY_FOUR))
                temp_value = 4;
            if (rl::IsKeyPressed(rl::KEY_FIVE))
                temp_value = 5;
            if (rl::IsKeyPressed(rl::KEY_SIX))
                temp_value = 6;
            if (rl::IsKeyPressed(rl::KEY_SEVEN))
                temp_value = 7;
            if (rl::IsKeyPressed(rl::KEY_EIGHT))
                temp_value = 8;
            if (rl::IsKeyPressed(rl::KEY_NINE))
                temp_value = 9;

            if (temp_value != -1)
                addCardSend(PlayingCard(rand()%999999, temp_value, 1, _x - card_dims_x/2, _y - card_dims_y/2, false));
        }
        else if (rl::IsKeyPressed(rl::KEY_SPACE))
        {
            for (int i = cards_on_ground.size() - 1; i >= 0; i--)
            {
                if (rl::CheckCollisionRecs({_x-4*SPRITE_SCALE, _y-4*SPRITE_SCALE, (float)ratSheet.frameWidth+2*SPRITE_SCALE, (float)ratSheet.frameHeight+2*SPRITE_SCALE}, {(float)cards_on_ground[i].x, (float)cards_on_ground[i].y, card_dims_x, card_dims_y}))
                {
                    fb_builder.Clear();
                    auto card = GS::CreatePlayingCard(fb_builder, FLAG_PLAYINGCARD_DATA, FLAG_PLAYINGCARD_UPDATE, cards_on_ground[i].unique_id, cards_on_ground[i].value, cards_on_ground[i].color, cards_on_ground[i].x, cards_on_ground[i].y, !cards_on_ground[i].flipped);
                    fb_builder.Finish(card);
                    enet_peer_send(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
                    break;
                }
            }
        }
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
            
            rl::DrawTextureEx(bgTexture, {bg_list[i].x - _camera_x, bg_list[i].y - _camera_y}, 0, SPRITE_SCALE, {255,255,255,255});
        }
    }

    void drawCard(const PlayingCard& card, float size_multiplier)
    {
        rl::DrawTexturePro(
            TEX_CARDS_SHEET, 
            {card.flipped ? 0 : card.value * (card_dims_x/SPRITE_SCALE), card.color * card_dims_y/SPRITE_SCALE, card_dims_x/SPRITE_SCALE, card_dims_y/SPRITE_SCALE}, 
            {(float)card.x - _camera_x, (float)card.y - _camera_y, card_dims_x*size_multiplier, card_dims_y*size_multiplier}, 
            {0,0}, 0, {255,255,255,255});
    }

    void drawCard(const PlayingCard& card, rl::Vector2 position, float size_multiplier)
    {
        rl::DrawTexturePro(
            TEX_CARDS_SHEET, 
            {card.flipped ? 0 : card.value * (card_dims_x/SPRITE_SCALE), card.color * card_dims_y/SPRITE_SCALE, card_dims_x/SPRITE_SCALE, card_dims_y/SPRITE_SCALE}, 
            {(float)position.x, (float)position.y, card_dims_x*size_multiplier, card_dims_y*size_multiplier}, 
            {0,0}, 0, {255,255,255,255});
    }

    void render()
    {
        rl::BeginDrawing();
            rl::ClearBackground(colorBg);
            fixDrawBackground();

            // cards on ground
            for (int i = 0; i < cards_on_ground.size(); i++)
            {
                drawCard(cards_on_ground[i], 1);
            }

            // draw players
            for (int i = 0; i < players_show.size(); i++)
            {
                rl::Vector2 msgTextSize = rl::MeasureTextEx(font2, players_show[i].message.c_str(), 35, 0);
                if (players_show[i].id == _id)
                {
                    rl::Vector2 pDrawPos = {_x - _camera_x, _y - _camera_y};
                    ratSheet.draw({pDrawPos.x, pDrawPos.y}, _rotation);
                    rl::DrawTextPro(font2, players_show[i].message.c_str(), {pDrawPos.x - msgTextSize.x/2, pDrawPos.y - 60}, {0,0}, 0, 35, 0, {255,255,255,255});
                    continue;
                }
                rl::Vector2 pDrawPos = {players_show[i].x - _camera_x, players_show[i].y - _camera_y};
                drawSheetFrame(TEX_RATS[players_show[i].rat_type], players_show[i].frame, ratSheet.frameWidth, ratSheet.frameHeight, pDrawPos, players_show[i].rotation, ratSheet.scale, ratSheet.origin);
                rl::DrawTextPro(font2, players_show[i].message.c_str(), {pDrawPos.x - msgTextSize.x/2, pDrawPos.y - 60}, {0,0}, 0, 35, 0, {255,255,255,255});
            }

            // cards on hand
            int hover_card = -1;
            for (int i = cards_on_hand.size()-1; i >= 0; i--)
            {
                int render_card = cards_on_hand.size()-1-i;
                if (rl::CheckCollisionPointRec(rl::GetMousePosition(), {(float)cards_on_hand[i].x, (float)cards_on_hand[i].y, card_dims_hand.x, card_dims_hand.y}) && hover_card == -1)
                {
                    if (rl::IsKeyPressed(rl::KEY_SPACE))
                        cards_on_hand[i].flipped = !cards_on_hand[i].flipped;
                    hover_card = i;
                }

                float total_width = cards_on_hand.size() * cards_on_hand_space + card_dims_hand.x/2.f;
                if (total_width >= WINW)
                {
                    cards_on_hand_space *= 0.99;
                    total_width = cards_on_hand.size() * cards_on_hand_space;
                }
                cards_on_hand[render_card].x = WINW/2 - total_width/2 + cards_on_hand_space*render_card;
                cards_on_hand[render_card].y = WINH - card_dims_y - 50;
                drawCard(cards_on_hand[render_card], rl::Vector2{(float)cards_on_hand[render_card].x, (float)cards_on_hand[render_card].y}, 1.5f);
            }
            if (hover_card != -1)
                drawCard(cards_on_hand[hover_card], rl::Vector2{(float)cards_on_hand[hover_card].x - (card_dims_hover.x-card_dims_hand.x)/2, (float)cards_on_hand[hover_card].y - (card_dims_hover.y-card_dims_hand.y)/2}, 1.7f);
            if (moving_card)
                drawCard(card_moving, {rl::GetMousePosition().x - card_dims_drag.x/2, rl::GetMousePosition().y - card_dims_drag.y/2}, 1.3f);

            std::string coordsText = std::to_string(ftint(_x/10.f)) + "; " + std::to_string(ftint(-_y/10.f));
            rl::DrawTextPro(font, coordsText.c_str(), {0,0}, {0,0}, 0, 30, 0, {255,255,255,255});

            if (isTypingMessage)
            {
                rl::Vector2 msgTextSize = rl::MeasureTextEx(font2, typeMessage.c_str(), 100, 0);
                rl::DrawTextPro(font2, typeMessage.c_str(), {WINW/2.f - msgTextSize.x/2, 50}, {0,0}, 0, 100, 0, {255,255,255,255});
                rl::DrawRectangle(WINW/2.f + msgTextSize.x/2, 50 + msgTextSize.y - 20, 25, int(rl::GetTime()*5.0)%2 * 5.f, {255,255,255,200});
            }
            
        rl::EndDrawing();
    }
};