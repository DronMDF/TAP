
#pragma once
#include <core/Socket.h>

struct SocketTest : public Socket {
	int fd;
	SocketTest(int fd) : fd(fd) {};
	virtual int getDescriptor() const { return fd; };
	virtual std::vector<uint8_t> recv(size_t) { return {}; };
	virtual size_t send(const std::vector<uint8_t> &) { return 0; };
	// New API
	virtual void recv() {};
	virtual void send() {};
};
