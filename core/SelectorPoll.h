
#pragma once
#include <map>
#include "Selector.h"

class SelectorPoll : public Selector {
public:
	SelectorPoll();
	SelectorPoll(int n);
	
	virtual void addSocket(const std::shared_ptr<Socket> &socket);
	virtual void removeSocket(const std::shared_ptr<Socket> &socket);
	virtual void proceed();

private:
	std::map<int, std::shared_ptr<Socket>> sockets;
};
