
#pragma once
#include <set>
#include <memory>

class Socket;

class Selector {
public:
	virtual ~Selector() {};

	// New API for Socket
	virtual void setSocket(unsigned idx, const std::shared_ptr<const Socket> &socket) = 0;
	virtual void select() = 0;
	virtual void selectRead(const std::function<void (int)> &callback) = 0;
	// Callback is a temporary name postfix
	virtual void selectWriteCallback(const std::function<void (int)> &callback) = 0;

	// Deprecated API
	virtual void setDescriptor(unsigned idx, int fd) = 0;
};
