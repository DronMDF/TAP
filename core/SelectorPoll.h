
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
	virtual bool selectWrite(unsigned idx);
	
private:
	std::vector<pollfd> rfds;
};
