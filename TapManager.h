
#pragma once
#include <list>

class ClientBuilder;

class TapManager {
public:
	TapManager(unsigned nth, const ClientBuilder &builder);
	virtual ~TapManager() {};
	
	void pressure();
	
private:
	virtual std::list<unsigned> selectIn();
	virtual void receive(unsigned i);
};
