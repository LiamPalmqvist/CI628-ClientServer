#include "Server.h"

Server::Server(const std::string& ipAddress, int port)
{
    std::thread listenThread(&Server::listenToPort, this, ipAddress, port);
    // detached from main thread - GitHub Copilot
    listenThread.detach();

    // Avoid busy-waiting here; if you need to wait for two clients,
    // consider using a condition_variable. For now, sleep briefly.
    while (threadListening)
    {
        if (clientsConnected == 2)
        {
            //game.playing = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        return;
    }
    else
    {
        str_port = std::to_string(port);
    }

    // setup variables for client connection
    int sockfd,
        newsockfd,
        portno,
        clilen;

    struct sockaddr_in serv_addr,
                       cli_addr;

    // Create a socket file descriptor for which to accept incoming connections
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cout << "Error opening socket" << std::endl;
        threadListening = false;
        return;
    }

    // clear the server's address to assign appropriate information
    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = std::atoi(str_port.data());

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the server information to the socket file descriptor
    // Cleaned up by GitHub Copilot
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "Error: Could not bind" << std::endl;
        threadListening = false;
        close(sockfd); // close the socket
        return; // and return
    }

    // Start listening for incoming connections
    if (listen(sockfd, 5) < 0) // check for errors
    {
        std::cout << "Error: listen failed" << std::endl;
        threadListening = false;
        close(sockfd); // close the socket
        return; // and return
    }

    clilen = sizeof(cli_addr);

    // While listening for incoming connections
    while (threadListening)
    {
        // if a client is requesting to connect, accept the connection
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, (socklen_t*)&clilen);
        // if the amount of clients connected already exceeds the limit
        if (newsockfd < 0)
        {
            std::cout << "error on accept" << std::endl;
            break;
        }

        // if break is used then it will not work properly
        if (clientsConnected >= 3)
        {
            // close the connection and continue
            close(newsockfd);
            continue; // change to continue
        }

        std::cout << "1. accepted connection" << std::endl;

        // and add to the clients connected
        incrementClientsConnected();

        // Create the client and add it to the list
        ClientData newClient;

        // assign the new client's socket file descriptor
        newClient.sockfd = newsockfd;

        int index = static_cast<int>(clientThreads.size());
        newClient.index = index;
        newClient.clientID = newClient.index + 1;
        newClient.active = true;
        newClient.assigned = false;
        std::cout << "2. Client ID: " << newClient.clientID << std::endl;

        clientThreads.push_back(std::move(newClient));

        // assign the player identity to the ID
        if (index == 0)
        {
            playerOneID = clientThreads[index].clientID;
        }
        else if (index == 1)
        {
            playerTwoID = clientThreads[index].clientID;
        }
        else
        {
            // extra safety: if >2, close
            close(clientThreads[index].sockfd);
            clientThreads[index].active = false;
        }

        // Create the thread with the reference to the client via index
        clientThreads[index].thread = std::thread([this, index]()
        {
            // capture index by value
            ClientData& clientRef = clientThreads[index];
            connectClient(clientRef.sockfd, clientRef);
            // mark inactive and cleanup
            clientRef.active = false;
        });

        // Clean up finished threads & inactive clients
        for (auto it = clientThreads.begin(); it != clientThreads.end();)
        {
            if (!it->active && it->thread.joinable())
            {
                it->thread.join();
                it = clientThreads.erase(it);
                // reassign indices
                for (size_t i = 0; i < clientThreads.size(); ++i)
                    clientThreads[i].index = static_cast<int>(i);
            }
            else
            {
                ++it;
            }
        }
    }

    close(sockfd);
}

int Server::connectClient(int sockfd, ClientData& client)
{
    int n;
    char buffer[256];
    std::string buffer_str;

    std::cout << "3. Client ID: " << client.clientID << std::endl;
    std::cout << "4. sockfd value: " << sockfd << std::endl;
    std::cout << "5. sockfd valid check: " << (sockfd >= 0 ? "yes" : "NO!") << std::endl;
    std::cout << "6. client.active: " << (client.active ? "yes" : "no") << std::endl;
    std::cout << "7. buffer address: " << (void*)buffer << std::endl;

    // Try to check if socket is actually valid
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);

    int* buffer_int;

    while (sockfd > 0 && client.active)
    {
        // sleep for ~60Hz
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        if (!client.assigned)
        {
            // Try assign client ID
            assignClientID(client);
            // if still not assigned, go around or break
            if (!client.assigned) {
                // If assignment failed and client was marked inactive, break
                if (!client.active) break;
                continue;
            }
        }
        else
        {
            bzero(buffer, sizeof(buffer));
            buffer_str.clear();

            n = recv(sockfd, buffer, sizeof(buffer), 0);

            if (n <= 0)
            {
                if (n == 0)
                {
                    std::cout << "8. Client " + std::to_string(client.clientID) + " disconnected" << std::endl;
                    decrementClientsConnected();
                    client.active = false;
                }
                else
                {
                    std::cout << "8. Error reading from socket: " << strerror(errno) << std::endl;
                    decrementClientsConnected();
                    client.active = false;
                }
                close(sockfd);
                break;
            }

            // Build string from the number of bytes actually read
            buffer_str.assign(buffer, static_cast<size_t>(n));
            if (!buffer_str.empty())
            {
                client.latestMessage = buffer_str;
            }

            decodeData(client);

            buffer_int = game.encodeData();

            // For logging/verification build a small readable string
            std::string str = "";
            for (int i = 0; i < 17; ++i)
            {
                if (i) str += ", ";
                str += std::to_string(buffer_int[i]);
            }
            // std::cout << "sending " << str << std::endl;
            broadcastIntMessage(buffer_int);
        }
    }

    return 1;
}

void Server::assignClientID(ClientData& client)
{
    int n;
    char buffer[256];
    bzero(buffer, sizeof(buffer));

    std::string buffer_str = std::to_string(clientsConnected);
    std::cout << "8. Clients connected: " << std::to_string(clientsConnected) << std::endl;
    std::cout << "9. Buffer string: " << buffer_str << std::endl;
    if (client.clientID == 0) client.clientID = client.index + 1;

    // Send ID to client
    n = send(client.sockfd, buffer_str.c_str(), buffer_str.length(), 0);
    if (n <= 0)
    {
        if (n == 0)
        {
            std::cout << "10. Client disconnected during send" << std::endl;
            decrementClientsConnected();
            client.active = false;
        }
        else
        {
            std::cout << "10. Error writing to socket: " << strerror(errno) << std::endl;
            decrementClientsConnected();
            client.active = false;
        }
        close(client.sockfd);
        return;
    }
    else std::cout << "10. No problem" << std::endl;

    // Recieve it back
    std::cout << "11. Assigned ID: " << buffer_str << std::endl;
    n = recv(client.sockfd, buffer, sizeof(buffer), 0);

    if (n <= 0)
    {
        if (n == 0)
        {
            std::cout << "13. Client disconnected" << std::endl;
            decrementClientsConnected();
            client.active = false;
        }
        else
        {
            std::cout << "13. Error reading from socket: " << strerror(errno) << std::endl;
            decrementClientsConnected();
            client.active = false;
        }
        close(client.sockfd);
        return;
    }
    else std::cout << "13. No problem" << std::endl;

    std::string received(buffer, static_cast<size_t>(n));
    std::cout << "12. Buffer: " << received << std::endl;

    int buffer_int = 0;
    try
    {
        buffer_int = std::stoi(received);
    }
    catch (const std::exception& e)
    {
        std::cout << "14. stoi failed: " << e.what() << ". Received data: '" << received << "'" << std::endl;
        // consider closing connection or retrying protocol
        close(client.sockfd);
        client.active = false;
        decrementClientsConnected();
        return;
    }

    std::cout << "14. " << received << std::endl;
    if (buffer_int == client.clientID)
    {
        client.returnedClientID = client.clientID;
        std::cout << "15. Client number assigned" << std::endl;
    }
    else
    {
        std::cout << "15. Client number was not assigned (client returned " << buffer_int << ")" << std::endl;
        return;
    }

    // Send out acknowledgement
    buffer_str = "ACK";
    n = send(client.sockfd, buffer_str.c_str(), buffer_str.length(), 0);

    if (n <= 0)
    {
        if (n == 0)
        {
            std::cout << "16. Client disconnected" << std::endl;
            decrementClientsConnected();
            client.active = false;
        }
        else
        {
            std::cout << "16. Error writing to socket: " << strerror(errno) << std::endl;
            decrementClientsConnected();
            client.active = false;
        }
        close(client.sockfd);
        return;
    }

    client.assigned = true;
    std::cout << "\n\n" << std::endl;
}

// Claude code
void Server::broadcastStringMessage(const std::string message, long excludeClientId)
{
    std::lock_guard lock(clientsMutex);

    for (auto& client : clientThreads)
    {
        if (client.active && client.clientID != excludeClientId)
        {
            if (client.writeStringToConnection(message))
                break;

            decrementClientsConnected();
            client.active = false;
            close(client.sockfd);
            return;
        }
    }
}

void Server::broadcastIntMessage(const int* message, long excludeClientId)
{
    std::lock_guard lock(clientsMutex);

    for (auto& client : clientThreads)
    {
        if (client.active && client.clientID != excludeClientId)
        {
            if (!client.writeIntToConnection(message))
            {
                decrementClientsConnected();
                client.active = false;
                close(client.sockfd);
                return;
            }
        }
    }
}

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

void Server::decodeData(ClientData& client)
{
    if (client.clientID == playerOneID)
    {
        if (clientThreads[client.index].latestMessage.size() >= 2)
        {
            p1Keys[0] = (clientThreads[client.index].latestMessage[0] == '1');
            p1Keys[1] = (clientThreads[client.index].latestMessage[1] == '1');
        }
    }
    else if (client.clientID == playerTwoID)
    {
        if (clientThreads[client.index].latestMessage.size() >= 2)
        {
            p2Keys[0] = (clientThreads[client.index].latestMessage[0] == '1');
            p2Keys[1] = (clientThreads[client.index].latestMessage[1] == '1');
        }
    }
}

// Claude code
bool ClientData::writeIntToConnection(const int* message) const
{
    // Assuming we always send 17 ints (adjust if different)
    const size_t totalBytes = 17 * sizeof(int);
    const char* data = reinterpret_cast<const char*>(message);
    size_t totalSent = 0;

    while (totalSent < totalBytes)
    {
        ssize_t sent = send(sockfd, data + totalSent, totalBytes - totalSent, MSG_NOSIGNAL);
        if (sent <= 0)
        {
            if (sent == -1 && errno == EINTR) continue;
            std::cerr << "writeIntToConnection: send failed: " << strerror(errno) << std::endl;
            return false;
        }
        totalSent += static_cast<size_t>(sent);
    }

    return true;
}

bool ClientData::writeStringToConnection(const std::string message) const
{
    const char* message_char = message.c_str();
    const size_t messageLen = message.size();
    size_t totalSent = 0;

    while (totalSent < messageLen)
    {
        const ssize_t sent = send(sockfd, message_char + totalSent, messageLen - totalSent, MSG_NOSIGNAL);

        if (sent < 0)
        {
            if (errno == EINTR) continue; // interrupted, retry
            std::cerr << "Send failed: " << strerror(errno) << std::endl;
            return false;
        }

        totalSent += static_cast<size_t>(sent);
    }

    return true;
}

void Server::incrementClientsConnected()
{
    std::lock_guard lock(clientsMutex);
    clientsConnected++;
}

void Server::decrementClientsConnected()
{
    std::lock_guard lock(clientsMutex);
    if (clientsConnected > 0) clientsConnected--;
}

// Cleaned up by GitHub Copilot