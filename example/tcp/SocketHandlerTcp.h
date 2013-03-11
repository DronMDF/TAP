
#pragma once
#include <core/SocketHandler.h>

class ClientTcp;

class SocketHandlerTcp : public SocketHandler
{
public:
	SocketHandlerTcp(ClientTcp *client);

	virtual void recv(const std::vector<uint8_t> &data) override;

private:
	ClientTcp *client;

	SocketHandlerTcp(const SocketHandlerTcp &);
	SocketHandlerTcp &operator =(const SocketHandlerTcp &);
};
