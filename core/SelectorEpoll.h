
#pragma once
#include <vector>
#include <sys/epoll.h>
#include "Selector.h"

class SelectorEpoll : public Selector {
public:
	SelectorEpoll(int n);
	virtual ~SelectorEpoll();
	
	virtual void setSocket(unsigned idx, const std::shared_ptr<const Socket> &socket);
	virtual void select();
	virtual void selectRead(const std::function<void (int)> &callback);
	virtual void selectWrite(const std::function<void (int)> &callback);

	virtual void setDescriptor(unsigned idx, int fd);

private:
	int epollfd;
	std::vector<int> fds;
	std::vector<epoll_event> events;
	int event_count;
};
