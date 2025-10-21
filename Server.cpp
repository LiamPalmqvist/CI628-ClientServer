//
// Created by Liam on 21/10/2025.
//

#include "Server.h"

#include <future>
#include <chrono>

Server::Server(std::string ipAddress, int port)
{
    std::string str_port; // for atoi();

    // First, validate the IP and Port
    if (!(validateIpAddress(ipAddress) && validatePortNumber(port)))
    {
	error("port ot IP address in wrong format");
    } else {
	str_port = std::to_string(port); // for atoi(); again
    }

    int sockfd, 	// stores value returned by socket
	newsockfd, 	// stores value returned by accpept system call
	portno, 	// stores the port number of the server
	clilen, 	// stores the size of the client's address - needed for system calls
	n;		// return value for the read() and write() calls.
    // sockfd and newsockfd are file descriptors which store the values returned by the socket system call and the accept system call
    // portno stores the port number on which the server accepts connections
    // clilen stores the size of the address of the client. This is needed to accept system call
    // n is the return value fr the read() and write() calls. i.e. it contains the number of characters read or written

    struct sockaddr_in serv_addr, // the server's address
		       cli_addr;  // the client's address
    // A sockaddr_in is a structure containint an internet address. This structire is defined in <netinet/in.h> and looks like this
    // struct sockaddr_in {
    // 	short 	sin_family;
    // 	u_short sin_port;
    // 	struct 	in_addr sin_addr;
    // 	char 	sin_zero[8]
    // };
    //
    // an in_addr structure, defined in the same header file, only contains one field. an unsigned long called s_addr

    char buffer[256];
    // the server reads characters from the socket connection into this buffer

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
	error("ERROR opening socket");
    // The socket() system call creates a new socket. It takes three arguments.
    // The first is the address domain of the socket, either AF_INET or  (IPv4, IPv6).
    // The second is the type of socket we want to open. There are two types of socket we can choose from: SOCK_STREAM and SOCK_DGRAM
    // The third is the protocol. If this argument is zero (which it should be usually), the operating system will choose the most appropriate protocol (TCP for SOCK_STREAM and UDP for SOCK_DGRAM)

    bzero((char *) &serv_addr, sizeof(serv_addr));
    // the function bzero() sets all values in a buffer to zero. It takes two arguments, the first is a pointer to the buffer and the second is the size of the buffer
    // This function sets all values in serv_addr to zero
    portno = std::atoi(str_port.data()); // the port number on which the server will listen for connections is passed in as an argument and this statement uses the atoi() function
			     // which converts this from a string to an integer

    serv_addr.sin_family = AF_INET; // same as earlier, we're setting the server's address domain to IPv4 using AF_INET
    serv_addr.sin_port = htons(portno); // the second field of the struct sockaddr_in is the sin_port. This is set to the value we set earlier
    serv_addr.sin_addr.s_addr = INADDR_ANY; // as mentioned before, the sin_addr contains a single value called s_addr which is literally un unsigned long. This contains the IP address for the host
					    // for server code, it will always be the IP address of the machine hosting the server

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
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


    std::list<std::thread> clients = {};
	std::list<std::shared_future<int>> futures;
    // NOTE: START OF Multithreading code!
    while (true)
    {
	    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
    	if (newsockfd < 0)
    		error("ERROR on accept");

    	// NOTE: GENERATED BY CLAUDE ===================================================================================
    	// Since we need information from this thread, we need to create a promise
    	std::promise<int> promise;
    	std::shared_future<int> future = promise.get_future().share();
    	futures.push_back(future); // store the shared_future

    	// Create thread with lambda to properly move the promise
    	clients.emplace_back([newsockfd](std::promise<int> prom) {
			int result = connectClient(newsockfd);  // assuming dostuff returns int
			prom.set_value(result);
		}, std::move(promise));
    	// NOTE: the accept() system call causes the process to block until a client to connect to the server.
    	// This is wakes up the process when a connection from a client has been
    	// successfully established. It returns a new file descriptor which all communication on this connection should go through.
    	// The second argument is a reference pointer to the address of the client on the other end of the connection, and the third argument is the size of this structure

    	// Check futures in a non-blocking way
    	for (auto it = futures.begin(); it != futures.end(); )
    	{
    		// Use wait_for with zero timeout to check without blocking
    		if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
    		{
    			int result = it->get();
    			if (result > 0)
    			{
    				std::cout << result << std::endl;
    			}
    			// Remove processed future from list
    			it = futures.erase(it);
    		}
    		else
    		{
    			++it;
    		}
    	}
    	// NOTE: END OF GENERATED BY CLAUDE ============================================================================
    }
	// We can only get here AFTER a client has established a connection
	// VARIABLES USED:
	// buffer
	// newsockfd
	// n
	
}


int Server::connectClient(int sock)
{
	int n;
	char buffer[256];

	while (sock > 0) {
		bzero(buffer,256);
		n = read(sock,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		printf("Here is the message: %s\n",buffer);
		n = write(sock,"I got your message",18);
		if (n < 0) error("ERROR writing to socket");
	}
	return 1;
}

// returns true is IP Address is valid, false if not
bool Server::validateIpAddress(const std::string &ipAddress) {
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
	return result != 0;
}


bool Server::validatePortNumber(const int &portNumber)
{
	return (portNumber > 0 && portNumber < 65535);
}

void Server::error(const char *msg)
{
	perror(msg);
	exit(1);
}
