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
        address.port = 25565;
        host = enet_host_create(&address, 32, 1, 0, 0);
        if (host == NULL)
            Log("error: create server");
        fb_builder = flatbuffers::FlatBufferBuilder(1024);
    }

    ENetHost* host;
    ENetAddress address;
    ENetEvent event;

    flatbuffers::FlatBufferBuilder fb_builder;

    std::vector<flatbuffers::Offset<GS::Player>> players_vector;

    // players_server vector with server positions
    std::vector<Player> players_server;

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
                Log("\taddress: " + std::to_string(event.peer->address.host));
                addClient(event.peer->incomingPeerID);
                broadcastState();
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                getClientInfo(event.packet->data);
                broadcastState();
                enet_packet_destroy(event.packet);
                break;
                
            case ENET_EVENT_TYPE_DISCONNECT:
                Lognl("disconnected:");
                Log("\tid: " + std::to_string(event.peer->incomingPeerID));
                Log("\taddress: " + std::to_string(event.peer->address.host));
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

    void getClientInfo(const void *buf)
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

    void broadcastState()
    {
        fb_builder.Clear();
        players_vector.clear();
        for (int i = 0; i < players_server.size(); i++)
        {
            players_vector.push_back(GS::CreatePlayer(fb_builder, players_server[i].id, players_server[i].x, players_server[i].y, players_server[i].rat_type, players_server[i].frame, players_server[i].rotation, fb_builder.CreateString(players_server[i].message)));
        }
        auto game_state = GS::CreateGameState(fb_builder, fb_builder.CreateVector(players_vector));
        fb_builder.Finish(game_state);
        enet_host_broadcast(host, 0, enet_packet_create(fb_builder.GetBufferPointer(), fb_builder.GetSize(), 0));
    }
};