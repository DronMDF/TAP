
#pragma once
#include "Socket.h"

struct in_addr;

class SocketTcp : public Socket {
public:
	SocketTcp(const in_addr &addr, unsigned port);

private:
	int sock;
};
