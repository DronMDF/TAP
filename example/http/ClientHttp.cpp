
#include "ClientHttp.h"

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

ClientHttp::ClientHttp(const in_addr &server, int port, const string &request)
	: addr(server), port(port), request(request), socket(), is_online(false),
	  start_time(time_point::max()), rx_bytes(0), status(0)
{
}

ClientHttp::~ClientHttp()
{
}

void ClientHttp::fixTimestamp(const string &what, ClientControl *control)
{
	const auto interval = chrono::high_resolution_clock::now() - start_time;
	control->trace(what, chrono::duration_cast<chrono::milliseconds>(interval).count());
	start_time = chrono::high_resolution_clock::now();
}

void ClientHttp::setTimeout(ClientControl *control, unsigned sec) const
{
	const auto wakeup_time = chrono::high_resolution_clock::now() + chrono::seconds(sec);
	control->setWakeupTime(wakeup_time);
}

int ClientHttp::getMain() const
{
	return socket->getDescriptor();
}

void ClientHttp::read(ClientControl *control)
{
	vector<uint8_t> buf = socket->recv(4096);

	if (buf.empty()) {
		control->trace("bytes", rx_bytes);
		if (status == 200) {
			fixTimestamp("recv", control);
		}
		
		control->trace("Closing connection by terminate");
		is_online = false;

		socket.reset();
		control->setSocket(socket);
		control->setStateOffline();
		return;
	}

	if (!is_online) {
		rx_bytes = 0;
		fixTimestamp("connect", control);
		
		is_online = true;
		control->trace("Client online");
		control->setStateOnline();
	}
	
	
	if (status == 0 and buf.size() >= 12) {
		const string status_text(reinterpret_cast<char *>(&buf[0]), 9, 3);
		status = boost::lexical_cast<unsigned>(status_text);
		if (status != 200) {
			control->trace("error code", status);
		}
	}
	
	rx_bytes += buf.size();
	setTimeout(control, 60);
}

void ClientHttp::timeout(ClientControl *control)
{
	if (socket) {
		control->trace("Closing connection by timeout");
		is_online = false;

		socket.reset();
		control->setSocket(socket);
		control->setStateOffline();
	}
}

void ClientHttp::action(ClientControl *control)
{
	if (!socket) {
		control->trace("Create connection");
		control->setStateConnecting();
		socket = make_shared<SocketTcp>(addr, port);
		start_time = chrono::high_resolution_clock::now();
		status = 0;
		
		control->trace("Send HTTP request");
		control->writeToMain(vector<uint8_t>(request.begin(), request.end()));
		
		setTimeout(control, 60);
	}
}
