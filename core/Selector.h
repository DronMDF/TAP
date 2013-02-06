
#pragma once
#include <set>
#include <memory>

class Socket;

class Selector {
public:
	virtual ~Selector() {};

	virtual void addSocket(const std::shared_ptr<Socket> &socket) = 0;
	virtual void proceed() = 0;
};

