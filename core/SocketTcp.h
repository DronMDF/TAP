
#pragma once
#include <memory>
#include "Socket.h"

struct in_addr;
class SocketHandler;

class SocketTcp : public Socket {
public:
	SocketTcp(const std::shared_ptr<SocketHandler> &handler);
	SocketTcp(const in_addr &addr, unsigned port);
	virtual ~SocketTcp();

	void bind(unsigned port);

	virtual int getDescriptor() const;

	virtual std::vector<uint8_t> recv(size_t size);
	virtual size_t send(const std::vector<uint8_t> &data);

	// New API
	virtual void recv();
	virtual void send();

private:
	int sock;
};
