
#pragma once
#include <core/Selector.h>

class SelectorTest : public Selector {
public:
	virtual void select() {};
	virtual void setDescriptor(unsigned, int) {};
	virtual int getDescriptor(unsigned) const { return -1; };
	virtual int selectRead() { return -1; };
	virtual int selectWrite(const std::set<unsigned> &) { return -1; };
	virtual void setSocket(unsigned, const std::shared_ptr<const Socket> &) {};
};
