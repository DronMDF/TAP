
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

	virtual void timeout(ClientControl *control) override;
	virtual void action(ClientControl *control) override;

	void read_notification(size_t readed);

private:
	void setTimeout(ClientControl *control, unsigned sec) const;
	void terminate(ClientControl* control);
	bool isOnline() const;

	const in_addr addr;
	const int port;

	std::shared_ptr<Socket> socket;
	
	time_point stamp;
	uint64_t readed;
};
