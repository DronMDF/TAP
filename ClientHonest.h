
#pragma once

#include "Client.h"

class ClientHonest :  public Client {
public:
	virtual void push();
	virtual void wakeup() {};
};
