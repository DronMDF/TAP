
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
	: addr(server), port(port), request(request), fd(-1)
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
	
	tracer->trace("Создан основной дескриптор");
	tracer->trace("Клиент в состоянии connecting");
	setState(CONNECTING);
	return fd;
}

void ClientHttp::readFromMain(ClientControl *control)
{
	vector<uint8_t> buf(4096);
	int rv = read(fd, &buf[0], buf.size());
	//tracer->trace((boost::format("Результат чтения из сокета: %1%") % rv).str());
	
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
	
	control->setTimeout(600);
}

void ClientHttp::timeout(ClientControl *control)
{
	if (fd != -1) {
		tracer->trace("Таймаут, шлем запрос");
		control->writeToMain(vector<uint8_t>(request.begin(), request.end()));
		control->setTimeout(600);
	}
}
