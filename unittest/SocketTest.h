
#pragma once
#include <core/Socket.h>

struct SocketTest : public Socket {
	int fd;
	SocketTest(int fd) : fd(fd) {};
	virtual int getDescriptor() const override { return fd; };
	virtual bool recv() override { return true; };
	virtual bool send() override { return true; };
};
