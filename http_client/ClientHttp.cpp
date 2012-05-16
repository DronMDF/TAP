
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdexcept>
#include "ClientHttp.h"

using namespace std;

ClientHttp::ClientHttp(const in_addr &server, int port, const string &request)
	: addr(server), port(port), fd(-1)
{
}

ClientHttp::~ClientHttp()
{
	close(fd);
}

int ClientHttp::createMainDescriptor()
{
	close(fd);
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		throw runtime_error("Не могу создать сокет");
	}
	
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = addr;
	if (connect(fd, (sockaddr *)&sa, sizeof(sa)) == -1) {
		throw runtime_error("Не могу подключиться к сокету");
	}
	
	return fd;
}

void ClientHttp::wakeup()
{
}
