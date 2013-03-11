
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
	: addr(server), port(port), socket(), is_online(false),
	  stamp(high_resolution_clock::now()), readed(0)
{
}

ClientTcp::~ClientTcp()
{
}

int ClientTcp::getMain() const
{
	return -1;
}

void ClientTcp::setTimeout(ClientControl *, unsigned sec) const
{
	const auto wakeup_time = high_resolution_clock::now() + seconds(sec);
	control.setWakeupTime(wakeup_time);
}

void ClientTcp::terminate(ClientControl *)
{
	control.removeSocket(socket);
	socket.reset();
	is_online = false;
	control.setStateOffline();
	readed = 0;
}

void ClientTcp::read(ClientControl *)
{
	const auto buf = socket->recv(1);
	if (buf.empty()) {
		control.trace("Closing connection (error or reset)");
		terminate(0);
		return;
	}

	if (!is_online) {
		is_online = true;
		control.trace("Client online");
		control.setStateOnline();
	}

	control.trace("read", buf.size());

	readed += buf.size();
//	const auto interval = high_resolution_clock::now() - stamp;
//	if (interval > seconds(10)) {
//		const auto ms = duration_cast<milliseconds>(interval).count();
//		const auto rate = readed * 8000 / ms;
//		control->trace("read rate (bit/sec)", rate);
//		stamp = high_resolution_clock::now();
//		readed = 0;
//	}

	setTimeout(0, 60);
}

void ClientTcp::read_notification(size_t rb)
{
	readed += rb;
	setTimeout(0, 60);
}

bool ClientTcp::write(ClientControl *, const vector<uint8_t> &)
{
	return true;
}

void ClientTcp::timeout(ClientControl *)
{
	if (socket) {
		control.trace("Closing connection by timeout", readed);
		terminate(0);
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
