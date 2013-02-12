
#pragma once
#include <poll.h>
#include <vector>
#include "Selector.h"

class SelectorPoll : public Selector {
public:
	SelectorPoll(int n);
	
	virtual void addSocket(const std::shared_ptr<Socket> &socket);
	virtual void removeSocket(const std::shared_ptr<Socket> &socket);
	virtual void proceed();

private:
	std::vector<pollfd> fds;
};

