
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <boost/format.hpp>
#include <core/Tracer.h>
#include <core/ClientControl.h>
#include "ClientHttp.h"

using namespace std;

ClientHttp::ClientHttp(const in_addr &server, int port, const string &request)
	: addr(server), port(port), request(request), request_sended(false), 
	  rx_start(time(0)), rx_bytes(0), fd(-1)
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
	
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = addr;
	if (connect(f, (sockaddr *)&sa, sizeof(sa)) == -1) {
		cerr << "Cannot connect socket: " << strerror(errno) << endl;
		close(f);
		return -1;
	}
	
	int flags = fcntl(f, F_GETFL, 0);
	fcntl(f, F_SETFL, flags | O_NONBLOCK);
	return f;
}

void ClientHttp::readFromMain(ClientControl *control)
{
	vector<uint8_t> buf(4096);
	int rv = read(fd, &buf[0], buf.size());
	
	if (rv <= 0) {
		close(fd);
		fd = createMainDescriptor();
		if (fd != -1) {
			tracer->trace("Клиент в состоянии connecting");
			rx_start = time(0);
			rx_bytes = 0;
			request_sended = false;
			control->setMainDescriptor(fd);
			control->setTimeout(0);
		}
		
		return;
	}

	if (getState() != ONLINE) {
		tracer->trace("Клиент в состоянии online");
		setState(ONLINE);
	}
	
	rx_bytes += rv;
	if (rx_bytes > 1024 * 1024) {
		time_t now = time(0);
		time_t delta = now - rx_start;
		tracer->trace_throughput(delta > 0 ? (rx_bytes / delta) : rx_bytes);
		rx_start = now;
		rx_bytes = 0;
	}
	
	control->setTimeout(60);
}

void ClientHttp::timeout(ClientControl *control)
{
	if (fd == -1) {
		return;
	}
	
	if (!request_sended) {
		tracer->trace("Таймаут, шлем запрос");
		control->writeToMain(vector<uint8_t>(request.begin(), request.end()));
		request_sended = true;
	} else {
		tracer->trace("Просто таймаут, запрос уже слали, поэтому ничего не шлем.");
	}
	
	control->setTimeout(60);
}
