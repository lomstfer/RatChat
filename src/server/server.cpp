#include "Server.hpp"
#include <fstream>

int main()
{
    std::ifstream configFile("config.txt");
    std::string portInput;
    std::string maxClientsInput;

    std::getline(configFile, portInput);
    portInput = portInput.substr(portInput.find(":") + 1, portInput.length());
    int port = DEFAULT_PORT;
    if (portInput != "")
        port = stoi(portInput);
    
    std::getline(configFile, maxClientsInput);
    maxClientsInput = maxClientsInput.substr(maxClientsInput.find(":") + 1, maxClientsInput.length());
    int maxClients = DEFAULT_MAXCLIENTS;
    if (maxClientsInput != "")
        maxClients = stoi(maxClientsInput);

    Server server = Server(port, maxClients);

    bool running = true;
    while (running)
    {
        server.update();
    }
}