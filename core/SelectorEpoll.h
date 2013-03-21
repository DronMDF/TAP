
#pragma once
#include <vector>
#include "Selector.h"

struct epoll_event;

class SelectorEpoll : public Selector {
public:
	SelectorEpoll();
	virtual ~SelectorEpoll() noexcept;

	virtual void addSocket(const std::shared_ptr<Socket> &socket) override;
	virtual void removeSocket(const std::shared_ptr<Socket> &socket) override;
	virtual void proceed() override;

protected:
	int recv(unsigned enable, int fd, int size = 65536) const;
	int send(unsigned enable, int fd, int size = 65536) const;

private:
	int epollfd;

	virtual void strategy(const std::vector<epoll_event> &evs);
};

