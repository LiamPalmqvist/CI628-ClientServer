#include "Client.h"
#include "Server.h"

bool validateIpAddress(const std::string &ipAddress);
bool validatePortNumber(const int &portNumber);
void error(const char *msg);

void setupServer(std::string ipAddress, int port);

void dostuff(int sock);

int main(int argc, char *argv[])
{
    std::cout << "Hello, World!" << std::endl;
    
    // test ip address
    //std::cout << validateIpAddress("1.1.1.1") << std::endl;
    //std::cout << validateIpAddress("Not an IP address") << std::endl;

    // test port number
    //std::cout << validatePortNumber(2000) << std::endl;
    //std::cout << validatePortNumber(-1) << std::endl;


    if (argc > 1 && !strcmp(argv[1], "client")) {
	Client("127.0.0.1", 2000);
    } else {
    	Server("127.0.0.1", 2000);
    }


    return 0;
}
