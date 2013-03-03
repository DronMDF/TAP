
#pragma once
#include <memory>
#include "Socket.h"

struct in_addr;
class SocketHandler;

class SocketTcp : public Socket {
public:
	SocketTcp(const std::shared_ptr<SocketHandler> &handler);
	SocketTcp(int s, const std::shared_ptr<SocketHandler> &handler);
	SocketTcp(const in_addr &addr, unsigned port);
	virtual ~SocketTcp();

	void bind(unsigned port);

	virtual int getDescriptor() const override;

	virtual std::vector<uint8_t> recv(size_t size) override;
	virtual size_t send(const std::vector<uint8_t> &data) override;

	// New API
	virtual void recv() override;
	virtual void send() override;

private:
	int sock;
	std::vector<uint8_t> send_buffer;
	std::shared_ptr<SocketHandler> handler;

	SocketTcp(const SocketTcp &);
	SocketTcp &operator =(const SocketTcp &);
};
