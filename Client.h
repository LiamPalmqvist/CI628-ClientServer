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
public:
    std::thread clientThread;
    bool threadRunning;

    Client(std::string ipAddress, int port);
    void pingServer(int sock);
    int setupSDL();
    bool validateIpAddress(const std::string &ipAddress);
    bool validatePortNumber(const int &portNumber);
    static void error(const char *msg);
};


#endif //CLIENT_CLIENT_H