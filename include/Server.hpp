#include "game_state_generated.h"
#include "Globals.hpp"
#include <iostream>
#include <enet/enet.h>
#include <string>
#include <vector>

struct Server
{
    Server()
    {
        Log("server");
        if (enet_initialize() != 0)
            Log("error: init enet");
        atexit(enet_deinitialize);
        address.host = ENET_HOST_ANY;
        address.port = 365;
        host = enet_host_create(&address, 32, 1, 0, 0);
        if (host == NULL)
            Log("error: create server");
        fb_builder = flatbuffers::FlatBufferBuilder(1024);

        for (int am = 0; am < 10; am++)
        {
            for (int i = 0; i < 13; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    cards_on_ground.emplace_back(rand()%999999, i+1, j, i*(card_dims_x+SPRITE_SCALE), j*(card_dims_y+SPRITE_SCALE), false);
                }
            }
        }
        
    }

    ENetHost* host;
    ENetAddress address;
    ENetEvent event;

    flatbuffers::FlatBufferBuilder fb_builder;

    // players_server vector with server positions
    std::vector<Player> players_server;
    std::vector<flatbuffers::Offset<GS::Player>> players_vector;

    std::vector<PlayingCard> cards_on_ground;
    std::vector<flatbuffers::Offset<GS::PlayingCard>> cards_on_ground_vector;

    float dt_time = 0;
    float dt_last_time = 0;
    float dt = 0;
    float time = 0;

    float broadcast_fps = 10;
    float broadcast_time = 0;
    bool broadcast_allowed = false;
    
    void update()
    {
        if (enet_host_service(host, &event, 1000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                Lognl("connected:");
                Log("\tid: " + std::to_string(event.peer->incomingPeerID));
                addClient(event.peer->incomingPeerID);
                broadcastState();
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                // to do: multiple different packets - for optimization and usability
                getClientInfo(event.packet->data);
                broadcastState();
                enet_packet_destroy(event.packet);
                break;
                
            case ENET_EVENT_TYPE_DISCONNECT:
                Lognl("disconnected:");
                Log("\tid: " + std::to_string(event.peer->incomingPeerID));
                removeClient(event.peer->incomingPeerID);
                broadcastState();
                event.peer->data = NULL;
                break;
            }
        }
    }

    void addClient(int id)
    {
        players_server.emplace_back(id, 0, 0, 0, 0, 0);
    }

    void removeClient(int id)
    {        
        for (int i = 0; i < players_server.size(); i++)
        {
            if (players_server[i].id == id)
            {
                players_server.erase(players_server.begin() + i);
                break;
            }
        }
    }

    void getClientInfo(const void* buf)
    {
        auto packetTypeHolder = flatbuffers::GetRoot<GS::PacketTypeHolder>(buf);

        if (packetTypeHolder->packet_type() == FLAG_PLAYER_DATA)
        {
            auto pData = flatbuffers::GetRoot<GS::Player>(buf);
        
            for (int i = 0; i < players_server.size(); i++)
            {
                if (players_server[i].id == pData->id())
                {
                    players_server[i].x = pData->x();
                    players_server[i].y = pData->y();
                    players_server[i].rat_type = pData->rat_type();
                    players_server[i].frame = pData->frame();
                    players_server[i].rotation = pData->rotation();
                    if (flatbuffers::GetString(pData->message()).length() > 0)
                    {
                        players_server[i].message = flatbuffers::GetString(pData->message());
                    }
                        
                }
            }
        }
        
        if (packetTypeHolder->packet_type() == FLAG_PLAYINGCARD_DATA)
        {
            auto pcData = flatbuffers::GetRoot<GS::PlayingCard>(buf);

            switch (pcData->command())
            {
            case FLAG_PLAYINGCARD_ADD:
                for (int i = 0; i < cards_on_ground.size(); i++)
                {
                    if (cards_on_ground[i].unique_id == pcData->unique_id()) {
                        cards_on_ground.erase(cards_on_ground.begin() + i);
                        break;
                    }
                }
                cards_on_ground.emplace_back(pcData->unique_id(), pcData->value(), pcData->color(), pcData->x(), pcData->y(), pcData->flipped());
                break;

            case FLAG_PLAYINGCARD_REMOVE:
                for (int i = 0; i < cards_on_ground.size(); i++)
                {
                    if (cards_on_ground[i].unique_id == pcData->unique_id())
                    {
                        cards_on_ground.erase(cards_on_ground.begin() + i);
                        break;
                    }
                }
                break;

            case FLAG_PLAYINGCARD_UPDATE:
                for (int i = 0; i < cards_on_ground.size(); i++)
                {
                    if (cards_on_ground[i].unique_id == pcData->unique_id())
                    {
                        cards_on_ground.erase(cards_on_ground.begin() + i);
                        cards_on_ground.emplace_back(pcData->unique_id(), pcData->value(), pcData->color(), pcData->x(), pcData->y(), pcData->flipped());
                    }
                }
                break;
            
            default:
                break;
            }
        }
    }

    void broadcastState()
    {
        fb_builder.Clear();

        players_vector.clear();
        for (int i = 0; i < players_server.size(); i++)
        {
            players_vector.push_back(GS::CreatePlayer(fb_builder, FLAG_PLAYER_DATA, players_server[i].id, players_server[i].x, players_server[i].y, players_server[i].rat_type, players_server[i].frame, players_server[i].rotation, fb_builder.CreateString(players_server[i].message)));
        }

        cards_on_ground_vector.clear();
        for (int i = 0; i < cards_on_ground.size(); i++)
        {
            cards_on_ground_vector.push_back(GS::CreatePlayingCard(fb_builder, FLAG_PLAYINGCARD_DATA, FLAG_PLAYINGCARD_ADD, cards_on_ground[i].unique_id, cards_on_ground[i].value, cards_on_ground[i].color, cards_on_ground[i].x, cards_on_ground[i].y, cards_on_ground[i].flipped));
        }

        auto game_state = GS::CreateGameState(fb_builder, fb_builder.CreateVector(players_vector), fb_builder.CreateVector(cards_on_ground_vector));
        fb_builder.Finish(game_state);
        enet_host_broadcast(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
    }
};