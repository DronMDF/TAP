
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

using namespace std;

ClientTcp::ClientTcp(const in_addr &server, int port)
	: addr(server), port(port), socket(), is_online(false)
{
}

ClientTcp::~ClientTcp()
{
}

int ClientTcp::getMain() const
{
	return -1;
}

void ClientTcp::setTimeout(ClientControl *control, unsigned sec) const
{
	const auto wakeup_time = chrono::high_resolution_clock::now() + chrono::seconds(sec);
	control->setWakeupTime(wakeup_time);
}

void ClientTcp::terminate(ClientControl* control)
{
	socket.reset();
	is_online = false;
	control->setSocket(socket);
	control->setStateOffline();
}

void ClientTcp::read(ClientControl *control)
{
	try {
		const auto buf = socket->recv();
		if (buf.empty()) {
			control->trace("Closing connection by reset");
			terminate(control);
			return;
		}

		if (!is_online) {
			is_online = true;
			control->trace("Client online");
			control->setStateOnline();
		}
		
		setTimeout(control, 60);
	} catch(const std::exception &) {
		control->trace("Closing connection by error");
		terminate(control);
	}
}

void ClientTcp::timeout(ClientControl *control)
{
	if (socket) {
		control->trace("Closing connection by timeout");
		terminate(control);
	}
}

void ClientTcp::action(ClientControl *control)
{
	if (!socket) {
		control->trace("Create connection");
		control->setStateConnecting();
		socket = make_shared<SocketTcp>(addr, port);
		control->setSocket(socket);
		setTimeout(control, 60);
	}
}
