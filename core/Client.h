
#pragma once
#include <functional>

// Абстрактный клиент нагрузки
class Client {
public:
	virtual ~Client() {}
	virtual void wakeup() = 0;
	
	virtual void push() __attribute__((deprecated)) {};
};

typedef std::function<Client *()> client_creator_t __attribute__((deprecated));
