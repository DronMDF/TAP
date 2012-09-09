
#pragma once
#include <string>
#include <memory>
#include <arpa/inet.h>
#include <core/Client.h>
#include <core/TimePoint.h>

class Socket;

class ClientTcp: public Client {
public:
	ClientTcp(const in_addr &server, int port);
	virtual ~ClientTcp();

	virtual void read(ClientControl *control);
	virtual void timeout(ClientControl *control);
	virtual void action(ClientControl *control);

private:
	int getMain() const;
	void setTimeout(ClientControl *control, unsigned sec) const;
	void terminate(ClientControl* control);

	const in_addr addr;
	const int port;

	std::shared_ptr<Socket> socket;
	
	bool is_online;
};
