
#pragma once
#include <functional>
#include <queue>
#include <vector>
#include "ClientControl.h"

class Socket;

// Parent of load clients
class Client {
public:
	Client();
	virtual ~Client();

	// TODO: Deprecaded API
	virtual void read(ClientControl *control) = 0;
	virtual bool write(ClientControl *control, const std::vector<uint8_t> &data);
	bool wantsToWrite() const;

	virtual void timeout(ClientControl *control) = 0;
	/// TapManager calls this method when the Сlient has no other events
	virtual void action(ClientControl *control);

	void setClientControl(const ClientControl &control);
		
protected:
	void writeToQueue(const std::vector<uint8_t> &data);
	void sendFromQueue(Socket *socket);

	ClientControl control;

private:
	Client(const Client&) = delete;
	Client &operator=(const Client&) = delete;
	
	virtual int getMain() const = 0;

	std::queue<std::vector<uint8_t>> queue;
};
