
#pragma once
#include <string>
#include <core/Client.h>

class ClientHttp: public Client {
public:
	ClientHttp(const std::string &request);
	virtual void wakeup();
};
