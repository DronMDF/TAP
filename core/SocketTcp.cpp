
#include "SocketTcp.h"

#include <stdexcept>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

using namespace std;

SocketTcp::SocketTcp(const in_addr &addr, unsigned port)
	: sock(socket(AF_INET, SOCK_STREAM, 0))
{
	if (sock == -1) {
		throw runtime_error(string("Cannot create socket: ") + strerror(errno));
	}

	const int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = addr;
	if (connect(sock, (sockaddr *)&sa, sizeof(sa)) == -1) {
		if (errno != EINPROGRESS) {
			throw runtime_error(string("Cannot connect socket: ") + strerror(errno));
		}
	}
}

SocketTcp::~SocketTcp()
{
	close(sock);
}

int SocketTcp::getDescriptor() const
{
	return sock;
}

vector<uint8_t> SocketTcp::recv()
{
	vector<uint8_t> data(4096);
	const int rv = read(sock, &data[0], data.size());
	if (rv < 0) {
		throw runtime_error(string("recv error: ") + strerror(errno));
	}

	data.resize(rv);
	return data;
}

size_t SocketTcp::send(const vector<uint8_t> &data)
{
	int rv = write(sock, &data[0], data.size());
	if (rv < 0) {
		throw runtime_error(string("send error: ") + strerror(errno));
	}

	return rv;
}
