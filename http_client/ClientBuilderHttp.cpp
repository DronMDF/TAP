
#include <string.h>
#include <sys/socket.h>
#include <stdexcept>
#include "ClientBuilderHttp.h"
#include "ClientHttp.h"

using namespace std;

ClientBuilderHttp::ClientBuilderHttp(const string &server, int port, const string &request)
	: addr({inet_addr(server.c_str())}), port(port), request(request)
{
}

int ClientBuilderHttp::createSocket() const
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		throw runtime_error("Не могу создать сокет");
	}
	
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = addr;
	if (connect(s, (sockaddr *)&sa, sizeof(sa)) == -1) {
		throw runtime_error("Не могу подключиться к сокету");
	}
	
	return s;
}

shared_ptr<Client> ClientBuilderHttp::createClient() const
{
	return make_shared<ClientHttp>(addr, port, request);
}
