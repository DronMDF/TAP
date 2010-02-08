
#pragma once

#include <boost/function.hpp>

// Абстрактный клиент СД
class Client {
public:
	virtual ~Client() {}
};

typedef boost::function<Client *()> client_creator_t;
