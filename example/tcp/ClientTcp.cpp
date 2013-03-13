
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
	setTimeout(0, 60);
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
			socket = make_shared<SocketTcp>(addr, port,
				make_shared<SocketHandlerTcp>(this));
			control.addSocket(socket);
			control.setStateConnecting();
			setTimeout(0, 60);
			control.trace("Goes connecting");
		} catch (const exception &e) {
			control.trace(e.what());
		}
	}
}
