
#pragma once
#include "../Selector.h"

class SelectorTest : public Selector {
public:
	virtual void setDescriptor(unsigned, int) {};
	virtual int getDescriptor(unsigned) const { return -1; };
	virtual int select() { return -1; };
};
