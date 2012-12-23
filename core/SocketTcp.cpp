
#include "SocketTcp.h"

#include <stdexcept>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

using namespace std;

SocketTcp::SocketTcp(const in_addr &addr, unsigned port)
	: sock(socket(AF_INET, SOCK_STREAM, 0))
{
	if (sock == -1) {
		return;
	}

	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = addr;
	if (connect(sock, (sockaddr *)&sa, sizeof(sa)) == -1) {
		if (errno != EINPROGRESS) {
			close(sock);
			sock = -1;
			return;
		}
	}

	const int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

SocketTcp::~SocketTcp()
{
	close(sock);
}

int SocketTcp::getDescriptor() const
{
	return sock;
}

vector<uint8_t> SocketTcp::recv(size_t size)
{
	vector<uint8_t> data(size);
	const int rv = read(sock, &data[0], data.size());
	if (rv < 0) {
		return {};
	}

	data.resize(rv);
	return data;
}

size_t SocketTcp::send(const vector<uint8_t> &data)
{
	int rv = write(sock, &data[0], data.size());
	if (rv < 0) {
		return 0;
	}

	return rv;
}
