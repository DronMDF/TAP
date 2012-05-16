
#pragma once
#include <functional>

// Абстрактный клиент нагрузки
class Client {
public:
	virtual ~Client() {}

	virtual int createMainDescriptor() = 0;
	
	virtual void wakeup() = 0;
};
