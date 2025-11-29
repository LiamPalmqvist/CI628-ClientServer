#include <sys/socket.h>
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h>
#include <netdb.h> // for gethostbyname
#include <iostream>
#include <thread>
#include <unistd.h>


class Client {
    // Private enumerator
    enum class State
    {
        unassigned,
        assigned
    };

    // Private variables
    bool connected;
    std::thread listeningThread;
    std::thread sendingThread;
    int clientID;
    State state;

    // Private functions
    static bool validateIpAddress(const std::string &ipAddress);
    static bool validatePortNumber(const int &portNumber);
    void listenToServer(int sockfd);
    void sendToServer(int sockfd);
public:
    // Public constructor
    Client(std::string ipAddress, int port);
};
