
#pragma once
#include <core/SocketHandler.h>

class ClientTcp;

class SocketHandlerTcp : public SocketHandler
{
public:
	SocketHandlerTcp(ClientTcp *client);

private:
	ClientTcp *client;

	SocketHandlerTcp(const SocketHandlerTcp &);
	SocketHandlerTcp &operator =(const SocketHandlerTcp &);
};
