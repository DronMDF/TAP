
#include "SocketTcp.h"

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
		return;
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

bool SocketTcp::recv()
{
	vector<uint8_t> data(1024);
	const int rv = read(sock, &data[0], data.size());

	if (rv <= 0) {
		handler->disconnect();
		return false;
	}

	data.resize(rv);
	handler->recv(data);
	return true;
}

bool SocketTcp::send()
{
	if (send_buffer.empty()) {
		send_buffer = handler->send();
	}

	int rv = write(sock, &send_buffer[0], send_buffer.size());
	if (rv <= 0) {
		handler->disconnect();
		return false;
	}

	send_buffer.erase(send_buffer.begin(), send_buffer.begin() + rv);
	return true;
}
