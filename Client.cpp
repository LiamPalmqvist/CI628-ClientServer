//
// Created by Liam on 21/10/2025.
//

#include "Client.h"

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
    setupSDL();
    return;
}

void Client::pingServer(int sock) {
	threadRunning = true;
	int n;
	char buffer[256];
	while(threadRunning) {
		printf("Please enter a message: ");
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);

		std::string strBuffer = buffer;
		if (strBuffer.contains("exit")) {
			threadRunning = false;
			break;
		}

		n = write(sock, buffer, strlen(buffer));
		if (n < 0)
			error("ERROR writing to socket");
		bzero(buffer, 256);
		read(sock, buffer, 255);
		if (n < 0)
			error("ERROR reading from socket");
		printf("%s\n", buffer);
	}
}

int Client::setupSDL() {

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
			if (event.type == SDL_QUIT) {
				std::cout << "exit" << std::endl;
				threadRunning = false;
				running = false;
			}
		}

		if (!threadRunning)
		{
			running = false;
		}

		// render something (White background)
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	clientThread.detach();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
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