
#pragma once
#include <poll.h>
#include <vector>
#include "Selector.h"

class SelectorPoll : public Selector {
public:
	SelectorPoll(int n);
	
	virtual void setSocket(unsigned idx, const std::shared_ptr<const Socket> &socket);
	virtual void select();
	virtual void selectRead(const std::function<void (int)> &callback);
	virtual void selectWrite(const std::function<void (int)> &callback);

	virtual void setDescriptor(unsigned idx, int fd);

private:
	std::vector<pollfd> fds;
};
