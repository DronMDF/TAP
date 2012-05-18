
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

int ClientHttp::createMainDescriptor(ClientControl *control)
{
	close(fd);
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		throw runtime_error("Не могу создать сокет");
	}
	
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = addr;
	if (connect(fd, (sockaddr *)&sa, sizeof(sa)) == -1) {
		throw runtime_error("Не могу подключиться к сокету");
	}
	
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	
	tracer->trace("Клиент в состоянии connecting");
	setState(CONNECTING);
	rx_start = time(0);
	rx_bytes = 0;
	request_sended = false;
	control->setTimeout(60);
	return fd;
}

void ClientHttp::readFromMain(ClientControl *control)
{
	vector<uint8_t> buf(4096);
	int rv = read(fd, &buf[0], buf.size());
	
	if (rv <= 0) {
		tracer->trace("Клиент в состоянии offline");
		control->setTimeout(0);
		setState(OFFLINE);
		throw runtime_error("Ошибка при выполнении read");
	}

	if (getState() != ONLINE) {
		tracer->trace("Клиент в состоянии online");
		setState(ONLINE);
	}
	
	rx_bytes += rv;
	if (rx_bytes > 64 * 1024) {
		time_t now = time(0);
		tracer->trace_throughput(rx_bytes / (now - rx_start));
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
