
#pragma once
#include <core/Socket.h>

struct SocketTest : public Socket {
	int fd;
	SocketTest(int fd) : fd(fd) {};
	virtual int getDescriptor() const override { return fd; };
	virtual int recv(int) override { return 0; };
	virtual int send(int) override { return 0; };
};
