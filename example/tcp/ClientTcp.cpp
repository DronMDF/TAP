
#include "ClientTcp.h"

#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <core/Tracer.h>
#include <core/ClientControl.h>
#include <core/SocketTcp.h>
#include "SocketHandlerTcp.h"

using namespace std;
using namespace std::chrono;

class SocketLimited : public SocketTcp
{
private:
	unsigned bandwidth;
	::time_point received_time;
public:
	SocketLimited(const in_addr &addr, unsigned port, const shared_ptr<SocketHandler> &handler,
			unsigned bandwidth)
		: SocketTcp(addr, port, handler), bandwidth(bandwidth),
		  received_time(high_resolution_clock::now())
	{
	}

	virtual int recv(int size) override
	{
		const auto now = high_resolution_clock::now();
		const auto interval = duration_cast<milliseconds>(now - received_time).count();
		received_time = now;
		int limited_size = bandwidth * interval / 8000;
		return SocketTcp::recv(max(min(size, limited_size), 1));
	}
};

ClientTcp::ClientTcp(const in_addr &server, int port)
	: addr(server), port(port), socket(), stamp(high_resolution_clock::now()), readed(0)
{
}

void ClientTcp::setTimeout(ClientControl *, unsigned sec) const
{
	const auto wakeup_time = high_resolution_clock::now() + seconds(sec);
	control.setWakeupTime(wakeup_time);
}

void ClientTcp::terminate()
{
	socket.reset();
	control.setStateOffline();
	readed = 0;
	setTimeout(0, 0);
}

bool ClientTcp::isOnline() const
{
	return readed > 0;
}

void ClientTcp::read_notification(size_t rb)
{
	if (!isOnline()) {
		control.setStateOnline();
		control.trace("Online");
	}

	readed += rb;
	setTimeout(0, 120);
}

void ClientTcp::disconnect_notification()
{
	control.trace("Closing connection by peer", readed);
	terminate();
}

void ClientTcp::timeout(ClientControl *)
{
	if (socket) {
		control.trace("Closing connection by timeout", readed);
		control.removeSocket(socket);
		terminate();
	}
}

void ClientTcp::action(ClientControl *)
{
	if (!socket) {
		try {
			// TODO: separate connect
			socket = make_shared<SocketLimited>(addr, port,
				make_shared<SocketHandlerTcp>(this), 10000);
			control.addSocket(socket);
			control.setStateConnecting();
			setTimeout(0, 60);
			control.trace("Goes connecting");
		} catch (const exception &e) {
			control.trace(e.what());
			control.removeSocket(socket);
			terminate();
		}
	}
}
