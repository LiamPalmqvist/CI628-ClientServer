#include "Client.h"
#include "Window.h"


Client::Client(const std::string ipAddress, const int port)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;
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
    if (!(validateIpAddress(ipAddress) && this->validatePortNumber(port)))
    {
        std::cout << "ERROR port number or ip wrong format" << std::endl;
        return;
    }
    std::string str_port = std::to_string(port);

    portno = std::atoi(str_port.data());
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cout << "Error creating socket" << std::endl;
        return;
    }

    // This function is included in the <netdb.h> library and allows the user to get an IP Address of a host name
    // if an IP address is supplied, no lookup is performed and it is copied into the h_name field
    server = gethostbyname(ipAddress.data());
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr,
          (char*)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    // This code sets the fields in serv_addr. Much of it is the same as in the server. However, because the field server->h_addr is a character string we use the function:
    // void bcopy(char *s1, char *s2, int length) which copies length bytes from s1 to s2

    if (connect(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "Error connecting" << std::endl;
        return;
    }

    connected = true;

    listeningThread = std::thread(&Client::listenToServer, this, sockfd);
    sendingThread = std::thread(&Client::sendToServer, this, sockfd);

    Window window;

    if (!window.windowIsOpen)
    {
        listeningThread.detach();
        sendingThread.detach();
        return;
    }
}

void Client::listenToServer(int sockfd)
{
    char buffer[256];
    std::string buffer_str;
    while (connected)
    {
        int n = recv(sockfd, buffer, sizeof(buffer), 0);

        if (n <= 0)
        {
            if (n == 0)
            {
                std::cout << "Client disconnected" << std::endl;
            }
            else
            {
                std::cout << "Error reading from socket" << std::endl;
            }
            connected = false;
            close(sockfd);
            return;
        }

        buffer_str = buffer;
        if (buffer_str.length() != 0)
        {
            std::cout << buffer_str << std::endl;
        }
        bzero(buffer_str.data(), buffer_str.length());
    }
}

void Client::sendToServer(int sockfd)
{
    int n;
    char message[256];

    while (connected)
    {
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;
        n = write(sockfd, &message, sizeof(message));

        if (n < 0)
        {
            std::cout << "Error writing to socket" << std::endl;
            close(sockfd);
            break;
        }
    }
}

bool Client::validateIpAddress(const std::string& ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0;
}

bool Client::validatePortNumber(const int& portNumber)
{
    return (portNumber > 0 && portNumber < 65535);
}

