
#pragma once
#include <arpa/inet.h>
#include <core/ClientBuilder.h>

class ClientBuilderHttp : public ClientBuilder {
public:
	ClientBuilderHttp(const ClientBuilderHttp&) = delete;
	ClientBuilderHttp(const std::string &server, int port, const std::string &request);
	virtual int createSocket() const;
	virtual std::shared_ptr<Client> createClient() const;
	
private:
	in_addr addr;
	int port;
	std::string request;
};
