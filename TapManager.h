
#pragma once
#include <poll.h>
#include <list>
#include <vector>
#include <memory>

class Client;
class ClientBuilder;

class TapManager {
public:
	TapManager(unsigned nth, const ClientBuilder &builder);
	virtual ~TapManager() {};
	
	void pressure();
	
private:
	std::vector<pollfd> pollfds;
	std::vector<unsigned> timeouts;
	std::vector<std::shared_ptr<Client>> clients;
	
	virtual std::list<unsigned> selectIn();
	virtual void receive(unsigned i);
};
