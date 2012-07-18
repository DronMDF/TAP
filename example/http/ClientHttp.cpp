
#include "ClientHttp.h"

#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <boost/format.hpp>
#include <core/Tracer.h>
#include <core/ClientControl.h>

using namespace std;

ClientHttp::ClientHttp(const in_addr &server, int port, const string &request)
	: addr(server), port(port), request(request), fd(-1), is_online(false), 
	  start_time(time_point::max()), rx_bytes(0)
{
}

ClientHttp::~ClientHttp()
{
	close(fd);
}

int ClientHttp::createMainDescriptor() const
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

void ClientHttp::fixTimestamp(const string &what, ClientControl *control)
{
	const auto interval = chrono::high_resolution_clock::now() - start_time;
	control->trace(what, chrono::duration_cast<chrono::milliseconds>(interval).count());
	start_time = chrono::high_resolution_clock::now();
}

bool ClientHttp::writeToMain(ClientControl *control, const std::vector<uint8_t> &data)
{
	const bool rv = Client::writeToMain(control, data);
	fixTimestamp("handshake", control);
	return rv;
}

void ClientHttp::setTimeout(ClientControl *control, unsigned sec) const
{
	const auto wakeup_time = chrono::high_resolution_clock::now() + chrono::seconds(sec);
	control->setWakeupTime(wakeup_time);
}

int ClientHttp::getMain() const
{
	return fd;
}

void ClientHttp::readFromMain(ClientControl *control)
{
	vector<uint8_t> buf(4096);
	int rv = read(fd, &buf[0], buf.size());
	
	if (rv <= 0) {
		control->trace("bytes", rx_bytes);
		fixTimestamp("recv", control);
		
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
	
	rx_bytes += rv;
	setTimeout(control, 60);
}

void ClientHttp::timeout(ClientControl *control)
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

void ClientHttp::action(ClientControl *control)
{
	if (fd == -1) {
		control->trace("Create connection");
		control->setStateConnecting();
		fd = createMainDescriptor();
		control->setMainDescriptor(fd);
		start_time = chrono::high_resolution_clock::now();
		
		control->trace("Send HTTP request");
		control->writeToMain(vector<uint8_t>(request.begin(), request.end()));
		
		setTimeout(control, 60);
	}
}

