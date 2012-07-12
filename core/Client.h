
#pragma once
#include <functional>
#include <vector>

class Tracer;
class ClientControl;

// Parent of load clients
class Client {
public:
	Client();
	virtual ~Client();

	virtual void readFromMain(ClientControl *control) = 0;
	virtual bool writeToMain(ClientControl *control, const std::vector<uint8_t> &data);
	virtual void timeout(ClientControl *control) = 0;
	
	/// TapManager calls this method when the Сlient has no other events
	virtual void action(ClientControl *control);
		
private:
	Client(const Client&) = delete;
	Client &operator=(const Client&) = delete;
	
	virtual int getMain() const = 0;
};
