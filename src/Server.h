#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h> // for sockaddr_in
#include <unistd.h>
#include <istream>
#include <thread>
#include <vector>
#include <mutex>
//#include <uuid/uuid.h>

#include "Game.h"

class ClientData {
public:

    // Public variables
    int clientID;
    int sockfd;
    int index;
    std::thread thread;
    bool active = true;
    bool assigned = false;
    int returnedClientID;
    std::string latestMessage = "";
    std::string previousMessage = "";

    // Public functions
    bool writeStringToConnection(const std::string message) const;
    bool writeIntToConnection(const int* message) const;
};

class Server {

    // Private variables
    bool threadListening = true;
    int clientsConnected = 0;
    std::vector<ClientData> clientThreads;
    // Claude correction code
    std::mutex clientsMutex;
    // End of Claude correction code

    int playerOneID;
    int playerTwoID;
    bool p1Keys[2] = {false, false};
    bool p2Keys[2] = {false, false};

    Game game;


public:

    // Public functions
    Server(const std::string &ipAddress, int port);
    void listenToPort(const std::string &ipAddress, int port);
    int connectClient(int sock, ClientData &client);
    static bool validateIpAddress(const std::string &ipAddress);
    static bool validatePortNumber(const int &portNumber);
    void broadcastStringMessage(const std::string message, long excludeClientId = -1);
    void broadcastIntMessage(const int* message, long excludeClientId = -1);
    void decodeData(int sockfd);
};

