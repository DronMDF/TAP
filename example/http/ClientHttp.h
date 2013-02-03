
#pragma once
#include <string>
#include <memory>
#include <arpa/inet.h>
#include <core/Client.h>
#include <core/TimePoint.h>

class ClientHttp: public Client {
public:
	ClientHttp(const in_addr &server, int port, const std::string &request);
	virtual ~ClientHttp();

	virtual void read(ClientControl *control);
	virtual void timeout(ClientControl *control);
	virtual void action(ClientControl *control);

private:
	int getMain() const;
	void setTimeout(ClientControl *control, unsigned sec) const;
	
	void fixTimestamp(const std::string &what, ClientControl *control);
	
	
	const in_addr addr;
	int port;
	const std::string request;

	std::shared_ptr<Socket> socket;
	
	bool is_online;
	
	time_point start_time;
	unsigned rx_bytes;
	
	unsigned status;
};
