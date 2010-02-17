
#pragma once

#include <boost/function.hpp>

// Абстрактный клиент СД
class Client {
public:
	virtual ~Client() {}
	virtual void push() = 0;
};

typedef boost::function<Client *()> client_creator_t;
