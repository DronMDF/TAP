
#pragma once
#include <string>
#include <arpa/inet.h>
#include <core/Client.h>

class ClientHttp: public Client {
public:
	ClientHttp(const in_addr &server, int port, const std::string &request);
	virtual ~ClientHttp();

	virtual int createMainDescriptor(ClientControl *control);
	virtual void readFromMain(ClientControl *control);
	virtual void timeout(ClientControl *control);

private:
	const in_addr addr;
	int port;
	const std::string request;
	bool request_sended;

	time_t rx_start;
	unsigned rx_bytes;
	
	int fd;
};
