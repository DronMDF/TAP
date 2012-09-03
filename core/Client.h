
#pragma once
#include <functional>
#include <queue>
#include <vector>

class ClientControl;
class Socket;

// Parent of load clients
class Client {
public:
	Client();
	virtual ~Client();

	virtual void read(ClientControl *control) = 0;
	virtual bool write(ClientControl *control, const std::vector<uint8_t> &data);
	virtual void timeout(ClientControl *control) = 0;
	
	/// TapManager calls this method when the Сlient has no other events
	virtual void action(ClientControl *control);
		
	bool wantsToWrite() const;

protected:
	void writeToQueue(const std::vector<uint8_t> &data);
	void sendFromQueue(Socket *socket);

private:
	Client(const Client&) = delete;
	Client &operator=(const Client&) = delete;
	
	virtual int getMain() const = 0;

	std::queue<std::vector<uint8_t>> queue;
};
