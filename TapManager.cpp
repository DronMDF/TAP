
#include "TapManager.h"
#include "ClientBuilder.h"

TapManager::TapManager(unsigned nth, const ClientBuilder &builder)
{
	for (unsigned i = 0; i < nth; i++) {
		builder.createSocket();
		builder.createClient();
	}
}
