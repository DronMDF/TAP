
#pragma once

class Selector {
public:
	virtual ~Selector() {};

	virtual void setDescriptor(unsigned idx, int fd) = 0;
	virtual int getDescriptor(unsigned idx) const = 0;
	virtual int select() = 0;
};
