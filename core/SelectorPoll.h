
#pragma once
#include <poll.h>
#include <vector>
#include "Selector.h"

class SelectorPoll : public Selector {
public:
	SelectorPoll(int n);
	
	virtual void setDescriptor(unsigned idx, int fd);
	virtual int getDescriptor(unsigned idx) const;
	virtual int selectRead();
	virtual int selectWrite(const std::set<unsigned> &intrest);
	
	virtual void setSocket(unsigned idx, const std::shared_ptr<const Socket> &socket);

private:
	std::vector<pollfd> rfds;
	unsigned rcursor;
	std::vector<pollfd> wfds;
	unsigned wcursor;
};
