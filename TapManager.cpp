
#include <boost/foreach.hpp>
#include "TapManager.h"
#include "ClientBuilder.h"

using namespace std;

TapManager::TapManager(unsigned nth, const ClientBuilder &builder)
{
	for (unsigned i = 0; i < nth; i++) {
		builder.createSocket();
		builder.createClient();
	}
}

void TapManager::pressure()
{
	BOOST_FOREACH(auto &i, selectIn()) {
		receive(i);
	}
}

list<unsigned> TapManager::selectIn()
{
	return {};
}

void TapManager::receive(unsigned i)
{
}
