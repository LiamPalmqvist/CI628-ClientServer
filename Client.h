//
// Created by Liam on 21/10/2025.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <exception>
#include <sys/_types/_pid_t.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <iostream>
#include <errno.h> // for perror()
#include <cstdlib> // for std::atoi()
#include <unistd.h> // for read()
#include <netdb.h> // for gethostbyname()
#include <list> // for list<T>
#include <SDL2/SDL.h>
#include <thread> // for threading

class Client
{
    enum class States {unassigned, assigned};
public:
    // Threading information
    std::thread clientThread;
    bool threadRunning;

    // global client information
    States state;
    int clientNumber;

    // Gameplay information
    bool keys[6] = {false, false, false, false, false, false};
    std::string keyNames[6] = {"w", "a", "s", "d", "lshift", "lctrl"};

    // Instance function
    Client(std::string ipAddress, int port);

    // =========== SDL FUNCTIONS ===========
    // sets up the SDL instance
    int setupSDL(int sockfd);

    // Gets the player inputs while the SDL instance is running
    void input(SDL_Event &event);
    // =========== SDL FUNCTIONS ===========

    //
    void pingServer(int sockfd);
    void checkForConnection(int sockfd, char buffer[256]);
    void sendInputs(int sockfd, char buffer[256]);

    // validates the IP address and Port number fed into the app
    bool validateIpAddress(const std::string &ipAddress);
    bool validatePortNumber(const int &portNumber);

    // generic error function
    static void error(const char *msg);
};


#endif //CLIENT_CLIENT_H