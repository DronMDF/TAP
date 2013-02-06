
#pragma once
#include <vector>
#include <map>
#include <sys/epoll.h>
#include "Selector.h"

class SelectorEpoll : public Selector {
public:
	SelectorEpoll(int n);
	virtual ~SelectorEpoll();
	
private:

public:
	virtual void addSocket(const std::shared_ptr<Socket> &socket);
	virtual void proceed();

private:
	int epollfd;
	std::map<int, std::shared_ptr<Socket>> sockets;
};

