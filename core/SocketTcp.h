
#pragma once
#include "Socket.h"

struct in_addr;

class SocketTcp : public Socket {
public:
	SocketTcp(const in_addr &addr, unsigned port);
	virtual ~SocketTcp();

	virtual int getDescriptor() const;

	virtual std::vector<uint8_t> recv(size_t size);
	virtual size_t send(const std::vector<uint8_t> &data);

private:
	int sock;
};
