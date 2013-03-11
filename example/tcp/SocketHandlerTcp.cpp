
#include "SocketHandlerTcp.h"
#include "ClientTcp.h"

using namespace std;

SocketHandlerTcp::SocketHandlerTcp(ClientTcp *client)
	: client(client)
{
}

void SocketHandlerTcp::recv(const vector<uint8_t> &data)
{
	client->read_notification(data.size());
}
