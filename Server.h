//
// Created by Liam on 21/10/2025.
//

#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include <exception>
#include <sys/_types/_pid_t.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <iostream>
#include <errno.h> // for perror()
#include <cstdlib> // for std::atoi()
#include <future>
#include <unistd.h> // for read()
#include <netdb.h> // for gethostbyname()
#include <list> // for list<T>
#include <SDL2/SDL.h>
#include <thread> // for threading
#include <list> // for lists
#include <future>
#include <chrono>
#include <memory>
#include <queue>

struct MessageQueue {
    std::queue<std::string> queue;
    std::mutex mtx; // MUTual EXclusion - ensures that only one thread at a time can access a shared resource

    // Pushes a message to the queue
    void push(const std::string& msg) {
        std::lock_guard lock(mtx);
        queue.push(msg);
    }

    // Tries to pop a message from the queue
    bool try_pop(std::string& msg) {
        std::lock_guard lock(mtx); // This makes sure that the message is locked to this thread
        if (queue.empty()) return false;
        msg = queue.front();
        queue.pop();
        return true;
    }
};

// Required for multithreading/converting
struct ClientData {
    std::thread thread;
    std::shared_ptr<MessageQueue> messages;
    bool active = true;

};

struct ClientGameplayData
{
    int paddle_size_1[2];
    int paddle_position_1[2];
    int paddle_size_2[2];
    int paddle_position_2[2];
    int ball_size[2];
    int ball_position[2];
    int ball_rotation; // between 0 and 359
};

class Server
{
public:
    bool p1keys[6] = {false, false, false, false, false, false};
    bool p2keys[6] = {false, false, false, false, false, false};

    ClientGameplayData data;
    int clientLength = 0;
    Server(const std::string &ipAddress, int port);
    void listenOnPort(const std::string &ipAddress, int port);
    void decodeMessage(std::string *message);
    int connectClient(int sock, const std::shared_ptr<MessageQueue>& msgQueue);
    static bool validateIpAddress(const std::string &ipAddress);
    static bool validatePortNumber(const int &portNumber);
    static void error(const char *msg);
};



#endif //CLIENT_SERVER_H
