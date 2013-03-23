
#pragma once
#include <memory>
#include "Socket.h"

struct in_addr;
class SocketHandler;

class SocketTcp : public Socket {
public:
	SocketTcp(const std::shared_ptr<SocketHandler> &handler);
	SocketTcp(int s, const std::shared_ptr<SocketHandler> &handler);
	SocketTcp(const in_addr &addr, unsigned port, const std::shared_ptr<SocketHandler> &handler);

	virtual ~SocketTcp() noexcept;

	void bind(unsigned port);

	virtual int getDescriptor() const override;

	virtual int recv(int size) override;
	virtual int send(int size) override;

private:
	int sock;
	bool connected;
	std::vector<uint8_t> send_buffer;
	std::shared_ptr<SocketHandler> handler;

	SocketTcp(const SocketTcp &) = delete;
	SocketTcp &operator =(const SocketTcp &) = delete;
};
