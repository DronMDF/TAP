
#pragma once
#include <core/Selector.h>

class SelectorTest : public Selector {
public:
	virtual void setSocket(unsigned, const std::shared_ptr<const Socket> &) {};
	virtual void select() {};
	virtual void selectRead(const std::function<void (int)> &) {};
	virtual void selectWrite(const std::function<void (int)> &) {};

	virtual void setDescriptor(unsigned, int) {};
};
