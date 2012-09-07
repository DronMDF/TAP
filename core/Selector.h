
#pragma once
#include <set>
#include <memory>

class Socket;

class Selector {
public:
	virtual ~Selector() {};

	// New API for Socket
	virtual void setSocket(unsigned idx, const std::shared_ptr<const Socket> &socket) = 0;
	virtual int selectRead() = 0;
	virtual int selectWrite(const std::set<unsigned> &intrest) = 0;

	// Deprecated API
	virtual void setDescriptor(unsigned idx, int fd) = 0;
	virtual int getDescriptor(unsigned idx) const = 0;
};
