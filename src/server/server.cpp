#include "Server.hpp"
#include <fstream>

int main()
{
    std::ifstream configFile("config.txt");
    std::string portInput;
    
    std::getline(configFile, portInput);
    portInput = portInput.substr(portInput.find(":") + 1, portInput.length());
    int port = -1;
    if (portInput != "")
        port = stoi(portInput);

    Server server = Server(port);

    bool running = true;
    while (running)
    {
        server.update();
    }
}