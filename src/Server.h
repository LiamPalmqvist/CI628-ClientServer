#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h> // for sockaddr_in
#include <unistd.h>
#include <istream>
#include <thread>
#include <vector>
#include <mutex>
//#include <uuid/uuid.h>

class ClientData {
public:

    // Public variables
    long clientID;
    int sockfd;
    std::thread thread;
    bool active = true;
    std::string latestMessage = "";
    std::string previousMessage = "";

    // Public functions
    bool writeToConnection(const std::string &message) const;
};

class Server {

    // Private variables
    bool threadListening = true;
    int clientsConnected = 0;
    std::vector<ClientData> clientThreads;
    // Claude correction code
    std::mutex clientsMutex;
    // End of Claude correction code

public:

    // Public functions
    Server(const std::string &ipAddress, int port);
    void listenToPort(const std::string &ipAddress, int port);
    int connectClient(int sock, ClientData &client);
    static bool validateIpAddress(const std::string &ipAddress);
    static bool validatePortNumber(const int &portNumber);
    void broadcastMessage(const std::string &message, long excludeClientId = -1);
};

