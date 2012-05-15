
#pragma once
#include <memory>

class Client;

class ClientBuilder {
public:
	virtual ~ClientBuilder() {};
	virtual int createSocket() const = 0;
	virtual std::shared_ptr<Client> createClient() const = 0;
};
