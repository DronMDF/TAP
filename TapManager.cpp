
#include <boost/foreach.hpp>
#include "Client.h"
#include "ClientBuilder.h"
#include "TapManager.h"

using namespace std;

TapManager::TapManager(unsigned nth, const ClientBuilder &builder)
	: pollfds(nth), timeouts(nth, 0), clients(nth)
{
	BOOST_FOREACH(auto &p, pollfds) {
		p.fd = builder.createSocket();
	}
	
	for (unsigned i = 0; i < nth; i++) {
		clients[i] = builder.createClient();
	}
}

void TapManager::pressure()
{
	while (true) {
		time_t now = time(0);
		
		for (unsigned i = 0; i < clients.size(); i++) {
			if (timeouts[i] < now) {
				clients[i]->wakeup();
			}
		}
	}
}

list<unsigned> TapManager::selectIn()
{
	return {};
}

void TapManager::receive(unsigned i)
{
}
