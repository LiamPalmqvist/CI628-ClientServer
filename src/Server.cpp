#include "Server.h"

Server::Server(const std::string& ipAddress, int port)
{
    std::thread listenThread(&Server::listenToPort, this, ipAddress, port);

    while (threadListening)
    {
        if (clientsConnected == 2)
        {
            //game.playing = true;
        }
    }
}

void Server::listenToPort(const std::string& ipAddress, int port)
{
    std::cout << "Hello" << std::endl;
    std::string str_port;

    // First, validate the IP and the Port Number
    if (!(validateIpAddress(ipAddress) && validatePortNumber(port)))
    {
        std::cout << "Error: IP Address or Port format incorrect" << std::endl;
        threadListening = false;
    }
    else
    {
        str_port = std::to_string(port);
    }

    int sockfd,
        newsockfd,
        portno,
        clilen;

    struct sockaddr_in serv_addr,
                       cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cout << "Error opening socket" << std::endl;
        threadListening = false;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = std::atoi(str_port.data());

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "Error: Could not bind" << std::endl;
        threadListening = false;
    }

    std::cout << "Started listening" << std::endl;
    listen(sockfd, 5);
    std::cout << "Listening for clients" << std::endl;

    clilen = sizeof(cli_addr);

    while (threadListening)
    {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, (socklen_t*)&clilen);
        if (clientsConnected >= 3)
        {
            close(newsockfd);
            break;
        }

        if (newsockfd < 0)
        {
            std::cout << "error on accept" << std::endl;
            break;
        }

        std::cout << "accepted connection" << std::endl;

        clientsConnected++;

        // Claude correction code
        // Create the client add it to the list *before* anything else
        ClientData newClient;

        // set the seed for the random client ID based on the time
        std::srand(std::time({}));
        newClient.clientID = rand();

        newClient.sockfd = newsockfd;

        int index = clientThreads.size();
        clientThreads.push_back(std::move(newClient));

        // Grab a reference to the existing client in the list
        ClientData& clientRef = clientThreads.back();
        // This saves us from accidentally deleting it

        // Create the thread with the reference to the existing client via index
        clientThreads[index].thread = std::thread([this, index]()
        {
            ClientData& clientRef = clientThreads[index];
            connectClient(clientRef.sockfd, clientRef);
        });
        // Claude correction code over

        // Old code:
        // ClientData newClient;
        // newClient.thread = std::thread([this, newsockfd, &newClient]()
        // {
        //     newClient.clientID = random();
        //     newClient.sockfd = newsockfd;
        //     connectClient(newsockfd, newClient);
        // });
        //
        // clientThreads.push_back(std::move(newClient));
        // End of old code

        for (auto client = clientThreads.begin(); client != clientThreads.end(); ++client)
        {
            if (!client->active && client->thread.joinable())
            {
                client->thread.join();
                client = clientThreads.erase(client);
            }
        }
    }
}

int Server::connectClient(int sockfd, ClientData &client)
{

    // close the port if there are already 2 clients connected
    if (clientsConnected >= 3)
    {
        close(sockfd);
        clientsConnected--;
    }

    std::cout << clientsConnected << std::endl;
    int n;
    char buffer[256];
    int clientNumber;
    std::string buffer_str;

    std::cout << "Client ID: " << client.clientID << std::endl;
    std::cout << "sockfd value: " << sockfd << std::endl;
    std::cout << "sockfd valid check: " << (sockfd >= 0 ? "yes" : "NO!") << std::endl;
    std::cout << "client.active: " << (client.active ? "yes" : "no") << std::endl;
    std::cout << "buffer address: " << (void*)buffer << std::endl;

    // Try to check if socket is actually valid
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
    std::cout << "getsockopt returned: " << retval << ", error: " << error << std::endl;

    int* buffer_int;

    while (sockfd > 0)
    {
        if (!client.assigned) {
            std::cout << "Client number is being assigned" << std::endl;
            const std::string assignment = std::to_string(client.clientID);
            n = send(sockfd, assignment.c_str(), sizeof(assignment), 0);
            std::cout << assignment << std::endl;
            n = recv(sockfd, buffer, sizeof(buffer), 0);
            std::cout << buffer << std::endl;
            buffer_str = buffer;
            if (buffer_str == assignment)
            {
                client.assigned = true;
                std::cout << "Client number assigned" << std::endl;
            } else
            {
                std::cout << "Client number was not assigned" << std::endl;
            }

            if (n <= 0)
            {
                if (n == 0)
                {
                    std::cout << "Client disconnected" << std::endl;
                    clientsConnected--;
                }
                else
                {
                    std::cout << "Error reading from socket" << std::endl;
                    clientsConnected--;
                }
                close(sockfd);
            }
        } else
        {
            bzero(buffer, sizeof(buffer));
            buffer_str = "";

            std::cout << "Client ID: " << client.clientID << std::endl;
            n = recv(sockfd, buffer, sizeof(buffer), 0);
            // std::cout << "Read from connection once" << std::endl;

            buffer_str = buffer;
            if (buffer_str.length() != 0)
            {
                std::cout << buffer_str << std::endl;
                client.latestMessage = buffer_str;
                buffer_str = "";
            }

            if (n <= 0)
            {
                if (n == 0)
                {
                    std::cout << "Client " + std::to_string(client.clientID) + " disconnected" << std::endl;
                    clientsConnected--;
                }
                else
                {
                    std::cout << "Error reading from socket" << std::endl;
                    clientsConnected--;
                }
                close(sockfd);
                break;
            }

            // if (game.playing)
            // {
                buffer_int = game.encodeData();

                std::string str = "";
                int i = 0;
                for (i; i < 16; i++)
                {
                    str += std::to_string(buffer_int[i]) + ", ";
                }
                str += std::to_string(buffer_int[i+1]);
                std::cout << "sending "  + str << std::endl;
                broadcastIntMessage(buffer_int);
            //}

            // if (client.latestMessage != client.previousMessage)
            // {
            //     std::cout << client.latestMessage << std::endl;
            //     client.previousMessage = client.latestMessage;
            //     for (int i = 0; i < clientsConnected; i++)
            //     {
            //         // if (clientThreads[i].active && clientThreads[i].clientID != client.clientID)
            //         // {
            //         std::cout << "Trying to write to client" << std::endl;
            //         clientThreads[i].writeToConnection(client.latestMessage);
            //         std::cout << "Written to socket" << std::endl;
            //         // }
            //     }
            // }
        }
    }

    return 1;
}

// Claude code
void Server::broadcastStringMessage(const std::string message, long excludeClientId)
{
    // A lock guard is created in order to automatically
    // lock and unlock a specific thread's data
    // This prevents data races and collisions from occuring on
    // multiple threads trying to access the same data
    std::lock_guard lock(clientsMutex);

    for (auto& client : clientThreads)
    {
        if (client.active && client.clientID != excludeClientId)
        {
            client.writeStringToConnection(message);
        }
    }
}

void Server::broadcastIntMessage(const int* message, long excludeClientId)
{
    // A lock guard is created in order to automatically
    // lock and unlock a specific thread's data
    // This prevents data races and collisions from occuring on
    // multiple threads trying to access the same data
    std::lock_guard lock(clientsMutex);

    for (auto& client : clientThreads)
    {
        if (client.active && client.clientID != excludeClientId)
        {
            client.writeIntToConnection(message);
        }
    }
}
// End of claude code


bool Server::validateIpAddress(const std::string& ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0;
}

bool Server::validatePortNumber(const int& portNumber)
{
    return (portNumber > 0 && portNumber < 65535);
}

// Old code
// bool ClientData::writeToConnection(std::string message)
// {
//     int n;
//     char buffer[256];
//
//     n = write(sockfd, message.c_str(), message.length());
//     if (n < 0)
//     {
//         std::cout << "Error writing to socket" << std::endl;
//         return false;
//     }
//     return true;
// }
// End of old code

// Claude code
bool ClientData::writeIntToConnection(const int* message) const
{
    int arr[17];
    for (int i = 0; i < 17; i++)
    {
        arr[i] = message[i];
    }
    //size_t totalSent = 0;
    const size_t messageLen = sizeof(arr);

    //while (totalSent < messageLen)
    //{
        //const ssize_t sent = send(sockfd, message + totalSent, messageLen - totalSent, MSG_NOSIGNAL);

        const ssize_t sent = write(sockfd, arr, messageLen);

        //totalSent += sent;
    //}

    return true;
}

bool ClientData::writeStringToConnection(const std::string message) const
{
    const char* message_char = message.c_str();
    const size_t messageLen = sizeof(message);
    size_t totalSent = 0;

    while (totalSent < messageLen)
    {
        const ssize_t sent = send(sockfd, message_char + totalSent, messageLen - totalSent, MSG_NOSIGNAL);

        if (sent < 0)
        {
            if (errno == EINTR) continue; // interrupted, retry
            std::cerr << "Send failed" << strerror(errno) << std::endl;
            return false;
        }

        totalSent += sent;
    }

    return true;
}
// End of Claude code