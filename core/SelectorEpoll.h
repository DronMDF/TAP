
#pragma once
#include <map>
#include "Selector.h"

class SelectorEpoll : public Selector {
public:
	SelectorEpoll();
	virtual ~SelectorEpoll();
	
	virtual void addSocket(const std::shared_ptr<Socket> &socket);
	virtual void removeSocket(const std::shared_ptr<Socket> &socket);
	virtual void proceed();

private:
	int epollfd;
	std::map<int, std::shared_ptr<Socket>> sockets;
};

