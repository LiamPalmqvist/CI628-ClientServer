//
// Created by Liam on 21/10/2025.
//

#include "Server.h"
#include "ServerGame.h"
#include <algorithm>
#include <string>

Server::Server(const std::string& ipAddress, int port)
{
    ServerGame game;
    std::thread listenThread(&Server::listenOnPort, this, ipAddress, port);
    //listenOnPort(ipAddress, port);
    int second = 1000;
    while (true)
    {
        if (clientLength == 2)
        {
            game.playing = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(second/60));
        //std::cout << clientLength << std::endl;
        if (game.playing)
            game.update(p1keys, p2keys);
        else
            std::cout << "Not enough players" << std::endl;
    }
}

void Server::listenOnPort(const std::string& ipAddress, int port) {
    std::string str_port; // for atoi();

    // First, validate the IP and Port
    if (!(validateIpAddress(ipAddress) && validatePortNumber(port)))
    {
        error("port or IP address in wrong format");
    }
    else
    {
        str_port = std::to_string(port); // for atoi(); again
    }

    int sockfd, // stores value returned by socket
        newsockfd, // stores value returned by accpept system call
        portno, // stores the port number of the server
        clilen; // stores the size of the client's address - needed for system calls

    // sockfd and newsockfd are file descriptors which store the values returned by the socket system call and the accept system call
    // portno stores the port number on which the server accepts connections
    // clilen stores the size of the address of the client. This is needed to accept system call
    // n is the return value fr the read() and write() calls. i.e. it contains the number of characters read or written

    struct sockaddr_in serv_addr, // the server's address
                       cli_addr; // the client's address
    // A sockaddr_in is a structure containint an internet address. This structire is defined in <netinet/in.h> and looks like this
    // struct sockaddr_in {
    // 	short 	sin_family;
    // 	u_short sin_port;
    // 	struct 	in_addr sin_addr;
    // 	char 	sin_zero[8]
    // };
    //
    // an in_addr structure, defined in the same header file, only contains one field. an unsigned long called s_addr

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    // The socket() system call creates a new socket. It takes three arguments.
    // The first is the address domain of the socket, either AF_INET or  (IPv4, IPv6).
    // The second is the type of socket we want to open. There are two types of socket we can choose from: SOCK_STREAM and SOCK_DGRAM
    // The third is the protocol. If this argument is zero (which it should be usually), the operating system will choose the most appropriate protocol (TCP for SOCK_STREAM and UDP for SOCK_DGRAM)

    bzero((char*)&serv_addr, sizeof(serv_addr));
    // the function bzero() sets all values in a buffer to zero. It takes two arguments, the first is a pointer to the buffer and the second is the size of the buffer
    // This function sets all values in serv_addr to zero
    portno = std::atoi(str_port.data());
    // the port number on which the server will listen for connections is passed in as an argument and this statement uses the atoi() function
    // which converts this from a string to an integer

    serv_addr.sin_family = AF_INET; // same as earlier, we're setting the server's address domain to IPv4 using AF_INET
    serv_addr.sin_port = htons(portno);
    // the second field of the struct sockaddr_in is the sin_port. This is set to the value we set earlier
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // as mentioned before, the sin_addr contains a single value called s_addr which is literally un unsigned long. This contains the IP address for the host
    // for server code, it will always be the IP address of the machine hosting the server

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    // Bind the server address to the socket file descriptor from earlier. In this case the address of the current host and port number on which the server will run.
    // It takes 3 arguments. The socket file descripter, the server IP address, and the size of the server address.
    // The second argument is cast as a pointer to a sockaddr. This is because the server address is stored as a sockaddr_in and must be cast to the correct type
    // This function can faile for a multitude of reasons but the most common one is because the port is already in use.

    listen(sockfd, 4);
    // Finally, we start listening for connections
    // The first argument is the socket file descriptor, and the second is the size of the backlog queue, i.e. the number of connections that can be
    // waiting while the process is handling a particular connection. This should be set to 5, the maximum size permitted by myst systems but since the game I'm
    // making will take a maximum of 4 players, this will be set to 4
    // If the first argument is a valid socket, this cannot fail, and so the code doesn't check for errors.

    // We need to get the length of the cli_addr
    clilen = sizeof(cli_addr);

    int clientsLength = 0;
    std::list<ClientData> clients = {};
    // NOTE: START OF Multithreading code!

    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, (socklen_t*)&clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        // add one to the client length
        clientLength++;

        // Create message queue for this client
        auto msgQueue = std::make_shared<MessageQueue>();

        // Create thread with message queue
        ClientData client;
        client.messages = msgQueue;
        client.thread = std::thread([this, newsockfd, msgQueue, &client]()
        {
            connectClient(newsockfd, msgQueue);
            client.active = false; // Mark as inactive when done
        });

        clients.push_back(std::move(client));

        // Check all clients for new messages
        for (auto it = clients.begin(); it != clients.end();)
        {
            std::string msg;
            // try to get the message from the client
            while (it->messages->try_pop(msg))
            {
                std::cout << msg << std::endl;
            }

            // Clean up disconnected clients
            if (!it->active && it->thread.joinable())
            {
                it->thread.join();
                it = clients.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}


int Server::connectClient(int sock, const std::shared_ptr<MessageQueue>& msgQueue)
{
    // close the socket if there are already 2 connections
    if (clientLength >= 3)
    {
        close(sock);
        clientLength--;
    }

    std::cout << clientLength << std::endl;
    int n; // return value for the read() and write() calls.
    char buffer[256];
    int clientNumber;
    // the server reads characters from the socket connection into this buffer

    bool clientAssigned = false;

    while (sock > 0)
    {
        bzero(buffer, 256);
        std::string input = "";

        n = read(sock, buffer, sizeof(buffer));

        //std::cout << buffer << std::endl;
        if (n <= 0)
        {
            if (n == 0)
            {
                printf("Client disconnected");
            }
            else
            {
                perror("ERROR reading from socket");
            }
            close(sock);
            break;
        }

        if (!clientAssigned) {
            std::string msg = "assigned: " + std::to_string(clientLength);
            std::cout << msg << std::endl;
            n = write(sock, msg.c_str(), sizeof(msg));
            if (n < 0)
            {
                perror("ERROR writing to socket");
                close(sock);
            }
            n = read(sock, buffer, sizeof(buffer));
            input = buffer;
            if (input.find("assigned: ") != std::string::npos)
                clientAssigned = true;
            else
            {
                std::cout << false << std::endl;
            }
        } else {
            //std::cout << "Client assigned" << std::endl;
            printf("%s\n", buffer);
            input = buffer;

            decodeMessage(&input);

            // Create JSON response and push to queue
            std::string json_response = "{\"message\": \"" + std::string(buffer) + "\"}";
            msgQueue->push(json_response);

            std::string message = "I got your message ";
            message += std::to_string(clientLength);
            n = write(sock, &message, sizeof(message));
            if (n < 0)
            {
                perror("ERROR writing to socket");
                close(sock);
                break;
            }
        }
    }

    n = read(sock, buffer, 255);

    std::cout << n << std::endl;

    return 1;
}

void Server::decodeMessage(std::string *message)
{
    int clientNumber = 0;

    std::cout << static_cast<int>(message->at(0)) << std::endl;

    std::bitset<8> number = std::stoi(*message) && 0b11000000;

    std::bitset<2> header = (static_cast<char>(number[0]) + static_cast<char>(number[1]));
    // Since this is only a 2 player game, we can use header.any()
    // to check which player we are. If a bit is positive, it will mean
    // that player is player 2

    if (header.any())
    {
        clientNumber = 1;
    }

    for (int i = 2; i < 6; i++)
    {
        switch (clientNumber)
        {
        case 0:
            p1keys[i] = number[i];
            break;
        case 1:
            p2keys[i] = number[i];
            break;
        default:
        }
    }
}

// returns true is IP Address is valid, false if not
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

void Server::error(const char* msg)
{
    perror(msg);
    exit(1);
}
