
#pragma once
#include <functional>
#include <list>
#include <vector>

class Tracer;
class ClientControl;

// Parent of load clients
class Client {
public:
	Client();
	virtual ~Client();

	// TODO: remove FromMain/ToMain
	virtual void read(ClientControl *control) = 0;
	// TODO: keep queue in private, change this in two methods isWantWrite and write
	virtual bool write(ClientControl *control, const std::vector<uint8_t> &data);
	virtual void timeout(ClientControl *control) = 0;
	
	/// TapManager calls this method when the Сlient has no other events
	virtual void action(ClientControl *control);
		
	bool wantsToWrite() const;

protected:
	void writeToQueue(const std::vector<uint8_t> &data);

private:
	Client(const Client&) = delete;
	Client &operator=(const Client&) = delete;
	
	virtual int getMain() const = 0;

	std::list<std::vector<uint8_t>> queue;
};
