
#pragma once
#include <poll.h>
#include <vector>
#include "Selector.h"

class SelectorPoll : public Selector {
public:
	SelectorPoll(int n);
	
	virtual void selectRead(const std::function<void (int)> &callback);
	virtual void selectWrite(const std::function<void (int)> &callback);

private:
	std::vector<pollfd> fds;

	// New selector API
public:
	virtual void addSocket(const std::shared_ptr<Socket> &socket);
	virtual void proceed();
};
