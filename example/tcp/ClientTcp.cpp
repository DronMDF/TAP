
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

using namespace std;

ClientTcp::ClientTcp(const in_addr &server, int port)
	: addr(server), port(port), fd(-1), is_online(false),
	  start_time(time_point::max()), rx_bytes(0), status(0)
{
}

ClientTcp::~ClientTcp()
{
	close(fd);
}

int ClientTcp::createMainDescriptor() const
{
	int f = socket(AF_INET, SOCK_STREAM, 0);
	if (f == -1) {
		cerr << "Cannot create socket: " << strerror(errno) << endl;
		return -1;
	}
	
	int flags = fcntl(f, F_GETFL, 0);
	fcntl(f, F_SETFL, flags | O_NONBLOCK);
	
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = addr;
	if (connect(f, (sockaddr *)&sa, sizeof(sa)) == -1) {
		if (errno != EINPROGRESS) {
			cerr << "Cannot connect socket: " << strerror(errno) << endl;
			close(f);
			return -1;
		}
	}
	
	return f;
}

void ClientTcp::fixTimestamp(const string &what, ClientControl *control)
{
	const auto interval = chrono::high_resolution_clock::now() - start_time;
	control->trace(what, chrono::duration_cast<chrono::milliseconds>(interval).count());
	start_time = chrono::high_resolution_clock::now();
}

void ClientTcp::setTimeout(ClientControl *control, unsigned sec) const
{
	const auto wakeup_time = chrono::high_resolution_clock::now() + chrono::seconds(sec);
	control->setWakeupTime(wakeup_time);
}

int ClientTcp::getMain() const
{
	return fd;
}

void ClientTcp::read(ClientControl *control)
{
	vector<uint8_t> buf(4096);
	int rv = ::read(fd, &buf[0], buf.size());
	
	if (rv <= 0) {
		control->trace("bytes", rx_bytes);
		if (status == 200) {
			fixTimestamp("recv", control);
		}
		
		control->trace("Closing connection by terminate");
		close(fd);
		fd = -1;
		is_online = false;
		control->setMainDescriptor(fd);
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
	
	
	if (status == 0 and rv >= 12) {
		const string status_text(reinterpret_cast<char *>(&buf[0]), 9, 3);
		status = boost::lexical_cast<unsigned>(status_text);
		if (status != 200) {
			control->trace("error code", status);
		}
	}
	
	rx_bytes += rv;
	setTimeout(control, 60);
}

void ClientTcp::timeout(ClientControl *control)
{
	if (fd > 0) {
		control->trace("Closing connection by timeout");
		close(fd);
		fd = -1;
		is_online = false;
		control->setMainDescriptor(fd);
		control->setStateOffline();
	}
}

void ClientTcp::action(ClientControl *control)
{
	if (fd == -1) {
		control->trace("Create connection");
		control->setStateConnecting();
		fd = createMainDescriptor();
		control->setMainDescriptor(fd);
		start_time = chrono::high_resolution_clock::now();
		status = 0;
		
		setTimeout(control, 60);
	}
}

