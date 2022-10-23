#include "Server.hpp"

int main()
{
    Server server = Server();

    bool running = true;
    while (running)
    {
        server.update();
    }
}