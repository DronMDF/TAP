
#pragma once
#include <set>
#include <memory>

class Socket;

class Selector {
public:
	virtual ~Selector() {};

	// New API for Socket
	virtual void setSocket(unsigned idx, const std::shared_ptr<const Socket> &socket) = 0;
	virtual void selectRead(const std::function<void (int)> &callback) = 0;
	// Callback is a temporary name postfix
	virtual void selectWrite(const std::function<void (int)> &callback) = 0;

	// New selector API
	virtual void addSocket(const std::shared_ptr<Socket> &socket) = 0;
	virtual void proceed() = 0;
};
