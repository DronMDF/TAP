
#pragma once
#include <string>
#include <arpa/inet.h>
#include <core/Client.h>

class ClientHttp: public Client {
public:
	ClientHttp(const in_addr &server, int port, const std::string &request);
	virtual ~ClientHttp();

	virtual void readFromMain(ClientControl *control);
	virtual void timeout(ClientControl *control);
	virtual void action(ClientControl *control);

private:
	int getMain() const;
	int createMainDescriptor() const;
	
	const in_addr addr;
	int port;
	const std::string request;

	time_t rx_start;
	unsigned rx_bytes;
	
	int fd;
};
