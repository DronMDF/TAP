
#pragma once
#include <map>
#include <memory>

class Socket;

class Selector {
public:
	Selector();
	virtual ~Selector();

	virtual void addSocket(const std::shared_ptr<Socket> &socket);
	virtual void removeSocket(const std::shared_ptr<Socket> &socket);
	size_t getCount() const;

	virtual void proceed() = 0;

protected:
	std::map<int, std::shared_ptr<Socket>> sockets;
};

