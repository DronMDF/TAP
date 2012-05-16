
#pragma once
#include <string>
#include <arpa/inet.h>
#include <core/Client.h>

class ClientHttp: public Client {
public:
	ClientHttp(const in_addr &server, int port, const std::string &request);
	virtual ~ClientHttp();

	virtual int createMainDescriptor();
	virtual void wakeup();
	
private:
	const in_addr addr;
	int port;
	int fd;
};
