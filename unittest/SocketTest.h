
#pragma once
#include <core/Socket.h>

struct SocketTest : public Socket {
	int fd;
	SocketTest(int fd) : fd(fd) {};
	virtual int getDescriptor() const override { return fd; };
	virtual std::vector<uint8_t> recv(size_t) override { return {}; };
	virtual size_t send(const std::vector<uint8_t> &) override { return 0; };
	// New API
	virtual bool recv() override { return true; };
	virtual bool send() override { return true; };
};
