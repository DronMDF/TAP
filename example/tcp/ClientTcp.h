
#pragma once
#include <string>
#include <arpa/inet.h>
#include <core/Client.h>
#include <core/TimePoint.h>

class ClientTcp: public Client {
public:
	ClientTcp(const in_addr &server, int port);
	virtual ~ClientTcp();

	virtual void read(ClientControl *control);
	virtual void timeout(ClientControl *control);
	virtual void action(ClientControl *control);

private:
	int getMain() const;
	int createMainDescriptor() const;
	void setTimeout(ClientControl *control, unsigned sec) const;

	void fixTimestamp(const std::string &what, ClientControl *control);

	const in_addr addr;
	const int port;

	int fd;
	
	bool is_online;
	
	time_point start_time;
	unsigned rx_bytes;
	
	unsigned status;
};
