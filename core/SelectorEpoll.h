
#pragma once
#include <vector>
#include <map>
#include <sys/epoll.h>
#include "Selector.h"

class SelectorEpoll : public Selector {
public:
	SelectorEpoll(int n);
	virtual ~SelectorEpoll();
	
	virtual void setSocket(unsigned idx, const std::shared_ptr<const Socket> &socket);
	virtual void selectRead(const std::function<void (int)> &callback);
	virtual void selectWrite(const std::function<void (int)> &callback);

private:
	int epollfd;
	std::vector<int> fds;
	std::vector<epoll_event> events;
	int event_count;

	// New selector API
public:
	virtual void addSocket(const std::shared_ptr<Socket> &socket);
	virtual void proceed();

private:
	std::map<int, std::shared_ptr<Socket>> sockets;
};
