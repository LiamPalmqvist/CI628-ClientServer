//
// Created by Liam on 21/10/2025.
//

#include "Client.h"

#include <algorithm>
#include <chrono>
#include <thread>

Client::Client(std::string ipAddress, int port)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    // the hostent structure is defined as follows:
    // struct hostent {
    // 		char	*h_name;	/* offifial name of host */
    // 		char	**h_aliases;	/* alias list */
    // 		int	h_addrtype;	/* host address type */
    // 		int	h_length;	/* lenfth of address */
    // 		char	**h_addr_list;	/* list of addresses from name server */
    // 	#define h_addr 	h_addr_list[0]  /* address, for backward compatibility */
    // }
    // it defines a host computer on the Internet. The members of this structure are
    // h_name		Official name of the host.
    // h_aliases	A zero terminated array of alternate names for the host
    // h_addrtype	The type of address being returned; currently always AF_INET
    // h_length		The length, in bytes, of the address
    // h_addr_list	A pointer to a list of network addresses for the names host. Host addresses are returned in the network byte order.
    // NOTE: h_addr is an alias for the first address in the array of network addresses

    // Validate port number and ip address
    if (!(validateIpAddress(ipAddress) && this->validatePortNumber(port))) error("ERROR port number or ip wrong format");
    std::string str_port = std::to_string(port);

    portno = std::atoi(str_port.data());
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd <0)
	error("ERROR opening socket");

    // This function is included in the <netdb.h> library and allows the user to get an IP Address of a host name
    // if an IP address is supplied, no lookup is performed and it is copied into the h_name field
    server = gethostbyname(ipAddress.data());
    if (server == NULL) {
	fprintf(stderr, "ERROR, no such host\n");
	exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
	    (char *)&serv_addr.sin_addr.s_addr,
	    server->h_length);
    serv_addr.sin_port = htons(portno);
    // This code sets the fields in serv_addr. Much of it is the same as in the server. However, because the field server->h_addr is a character string we use the function:
    // void bcopy(char *s1, char *s2, int length) which copies length bytes from s1 to s2

    if (connect(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	error("ERROR connecting");
    // The connect() function is called by the client to establish a connection to the server. Ita takes three arguments, the socket file descriptor, the address of the host to which it wants to connect (including the port number), and the size of the address. This function returns 0 on success and -1 on fail.

	clientThread = std::thread(&Client::pingServer, this, sockfd);

    // This is where I need to start threading
    // test setup SDL
    // setupSDL(); // must be on its own thread

    /*
    while(1)
    {
	printf("Please enter a message: ");
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	n = write(sockfd, buffer, strlen(buffer));
	if (n < 0)
	    error("ERROR writing to socket");
	bzero(buffer, 256);
	read(sockfd, buffer, 255);
	if (n < 0)
	    error("ERROR reading from socket");
	printf("%s\n", buffer);
    }
    */
    setupSDL(sockfd);
    return;
}

void Client::pingServer(int sockfd) {
	// setup default states
	state = States::unassigned;
	threadRunning = true;
	char buffer[256];
	uint8_t message;

	write(sockfd, "Ping", sizeof("Ping"));

	// set up the thread to check for input from server
	while(threadRunning) {

		switch (state) {
		case States::unassigned:
			std::cout << "Unassigned" << std::endl;
			checkForConnection(sockfd, buffer);
			break;
		case States::assigned:
			std::cout << "Assigned" << std::endl;
			sendInputs(sockfd, buffer);
			break;
		}

		//fgets(buffer, 255, stdin);

		/*
		std::string message = "{\"keys\":{";
		for (int i = 0; i < std::size(keys); i++)
		{
			message += "\"" + keyNames[i] + "\":\"" + (keys[i] ? "true" : "false") + "\"";
			if (i != std::size(keys) - 1)
			{
				message += ",";
			}
		}
		message += "}}";
		*/

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void Client::checkForConnection(int sockfd, char buffer[256])
{
	// Try to read from the socket
	int n = read(sockfd, buffer, 255);
	if (n < 0)
		error("ERROR reading from socket");
	// convert the buffer to a readable string
	std::string output = buffer;

	// check string for assignment message
	if (int index = output.find("assigned: "); index != std::string::npos) {
		std::cout << output.back() << std::endl;
		clientNumber = std::stoi(output.substr(output.size() - 1));
		state = States::assigned;
	}

	// make sure to clear the buffer
	bzero(buffer, 256);
}

void Client::sendInputs(int sockfd, char buffer[256])
{
	int n;
	uint8_t message = 0b00000000;

	// first, OR the client number with the message
	switch (clientNumber)
	{
	case 1:
		message |= 0b01000000;
		break;
	case 2:
		message |= 0b10000000;
		break;
	case 3:
		message |= 0b11000000;
		break;
	default:
		break;
	}

	// second, create mask from keys values
	std::string mask = "00";
	for (int i = 0; i < std::size(keys); i++)
	{
		if (keys[i]) {
			mask += "1";
		} else {
			mask += "0";
		}
	}

	const uint8_t mask2 = std::stoi(mask);
	message |= mask2;
	std::cout << message << std::endl;

	n = write(sockfd, &message, sizeof(message));
	std::cout << message << std::endl;
	//n = write(sock, buffer, strlen(buffer));
	if (n < 0)
		error("ERROR writing to socket");
	bzero(buffer, 256);

	// Read from the socket to the buffer
	read(sockfd, buffer, 255);
	if (n < 0)
		error("ERROR reading from socket");
	// and make changes

}

int Client::setupSDL(int sockfd) {

	// Check if we can create an SDL Window
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}

	// Start creating a window for SDL
	SDL_Window* window = SDL_CreateWindow(
		"Game",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		SDL_WINDOW_SHOWN
	);

	// if the window creation failed, print the error and exit
	if (!window) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 0;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	// This function creates a renderer for the window, using hardware acceleration
	// and VSync
	// -1 indicates that it will use the first available rendering driver

	// If the renderer creation fails, print the error and exit
	if (!renderer) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError();
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 0;
	}

	// Finally, we get to the main loop
	bool running = true;
	SDL_Event event;

	while (running) {
		// Process events
		while (SDL_PollEvent(&event)) {
			input(event);
			if (event.type == SDL_QUIT) {
				std::cout << "exit" << std::endl;
				threadRunning = false;
				running = false;
			}
		}

		//game->render();

		if (!threadRunning)
		{
			running = false;
		}

		// render something (White background)
		//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	clientThread.detach();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void Client::input(SDL_Event &event)
{
	switch (event.key.keysym.sym)
	{
	case SDLK_w:
		keys[0] = event.type == SDL_KEYDOWN;
		break;
	case SDLK_a:
		keys[1] = event.type == SDL_KEYDOWN;
		break;
	case SDLK_s:
		keys[2] = event.type == SDL_KEYDOWN;
		break;
	case SDLK_d:
		keys[3] = event.type == SDL_KEYDOWN;
		break;
	case SDLK_LSHIFT:
		keys[4] = event.type == SDL_KEYDOWN;
		break;
	case SDLK_LCTRL:
		keys[5] = event.type == SDL_KEYDOWN;
		break;
	case SDLK_ESCAPE:
		std::cout << "exit";
		threadRunning = false;
	default:
		break;
	}
}

// returns true is IP Address is valid, false if not
bool Client::validateIpAddress(const std::string &ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
	return result != 0;
}


bool Client::validatePortNumber(const int &portNumber)
{
	return (portNumber > 0 && portNumber < 65535);
}

void Client::error(const char *msg)
{
	perror(msg);
	exit(1);
}
