#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <iostream>
#include <errno.h> // for perror()
#include <cstdlib> // for std::atoi()
#include <unistd.h> // for read()
#include <netdb.h> // for gethostbyname()

bool validateIpAddress(const std::string &ipAddress);
bool validatePortNumber(const int &portNumber);
void setupServer(std::string ipAddress, int port);
void setupClient(std::string ipAddress, int port);
void error(char *msg);

int main(int argc, char *argv[])
{
    std::cout << "Hello, World!" << std::endl;
    
    // test ip address
    std::cout << validateIpAddress("1.1.1.1") << std::endl;
    std::cout << validateIpAddress("Not an IP address") << std::endl;

    // test port number
    std::cout << validatePortNumber(2000) << std::endl;
    std::cout << validatePortNumber(-1) << std::endl;


    if (argc > 1 && !strcmp(argv[1], "client")) {
	setupClient("127.0.0.1", 2000);
    } else {
    	setupServer("127.0.0.1", 2000);
    }


    return 0;
}

// returns true is IP Address is valid, false if not
bool validateIpAddress(const std::string &ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0;
}

bool validatePortNumber(const int &portNumber)
{
    return (portNumber > 0 && portNumber < 65535);
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// sets up a server
void setupServer(std::string ipAddress, int port)
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
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
    if (newsockfd < 0)
	error("ERROR on accept");
    // NOTE: the accept() system call causes the process to block until a client to connect to the server. 
    // This is wakes up the process when a connection from a client has been
    // successfully established. It returns a new file descriptor which all communication on this connection should go through.
    // The second argument is a reference pointer to the address of the client on the other end of the connection, and the third argument is the size of this structure
    

    // We can only get here AFTER a client has established a connection
    // VARIABLES USED:
    // buffer
    // newsockfd
    // n
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if (n <0) error("ERROR reading from socker");
    printf("Here is your message: %s\n", buffer);
    // initialise the buffer using bzero to set all bits in the buffer to 0
    // and then reads it from the new socket file descriptor
    // the output is stored in the variable n (as in if it succeeded or failed)
    // and buffer stores what was actually read from the connection
    // NOTE: read() will block until there is something to be read from the socket!

    n = write(newsockfd, "I got your message!", 19);
    if (n <0) error("ERROR writing to the socket");
}

void setupClient(std::string ipAddress, int port)
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
    if (!(validateIpAddress(ipAddress) && validatePortNumber(port))) error("ERROR port number or ip wrong format");
    std::string str_port = std::to_string(port);

    char buffer[256];
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
    return;

}
