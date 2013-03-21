
#include "SocketTcp.h"

#include <algorithm>
#include <stdexcept>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "SocketHandler.h"

using namespace std;

SocketTcp::SocketTcp(const shared_ptr<SocketHandler> &handler)
	: sock(socket(AF_INET, SOCK_STREAM, 0)), send_buffer(), handler(handler)
{
	if (sock == -1) {
		throw runtime_error(string("Cannot create socket: ") + strerror(errno));
	}

	const int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

SocketTcp::SocketTcp(int s, const shared_ptr<SocketHandler> &handler)
	: sock(s), send_buffer(), handler(handler)
{
	const int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

SocketTcp::SocketTcp(const in_addr &addr, unsigned port, const shared_ptr<SocketHandler> &handler)
	: sock(socket(AF_INET, SOCK_STREAM, 0)), send_buffer(), handler(handler)
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
			close(sock);
			sock = -1;
			throw runtime_error(string("Cannot connect socket: ") + strerror(errno));
		}
	}
}

SocketTcp::~SocketTcp() noexcept
{
	close(sock);
}

int SocketTcp::getDescriptor() const
{
	return sock;
}

void SocketTcp::bind(unsigned port)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (::bind(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
		throw runtime_error(string("Cannot bind socket: ") + strerror(errno));
	}

	// TODO: at this moment bind needed only for listener, so listen is here
	//	but in theory we can bind connected socket.
	if (listen(sock, 100) == -1) {
		throw runtime_error(string("Cannot listen socket: ") + strerror(errno));
	}
}

int SocketTcp::recv(int size)
{
	vector<uint8_t> data(size);
	const int rv = read(sock, &data[0], data.size());

	if (rv <= 0) {
		handler->disconnect();
		return -1;
	}

	data.resize(rv);
	handler->recv(data);
	return rv;
}

int SocketTcp::send(int size)
{
	if (send_buffer.empty()) {
		send_buffer = handler->send();
	}

	if (send_buffer.empty()) {
		// Nothing to send
		return 0;
	}

	int rv = write(sock, &send_buffer[0], min(send_buffer.size(), size_t(size)));
	if (rv <= 0) {
		handler->disconnect();
		return -1;
	}

	send_buffer.erase(send_buffer.begin(), send_buffer.begin() + rv);
	return rv;
}
