
#pragma once
#include <core/Selector.h>

class SelectorTest : public Selector {
public:
	virtual void setDescriptor(unsigned, int) {};
	virtual int getDescriptor(unsigned) const { return -1; };
	virtual int selectRead() { return -1; };
	virtual int selectWrite() { return -1; };
};
