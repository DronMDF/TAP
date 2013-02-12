
#pragma once
#include <core/Selector.h>

class SelectorTest : public Selector {
	virtual void addSocket(const std::shared_ptr<Socket> &) {};
	virtual void removeSocket(const std::shared_ptr<Socket> &) {};
	virtual void proceed() {};
};
