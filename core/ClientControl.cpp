
#include "ClientControl.h"
#include "TapManager.h"

ClientControl::ClientControl(TapManager *tapm, unsigned n)
	: tapm(tapm), n(n)
{
}

void ClientControl::writeToMain(const std::vector<uint8_t> &data) const
{
	tapm->writeToMain(n, data);
}

void ClientControl::setTimeout(unsigned timeout) const
{
	tapm->setTimeout(n, timeout);
}
