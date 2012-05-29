
#include "ClientControl.h"

#include <chrono>
#include "TapManager.h"

using namespace std;

ClientControl::ClientControl(TapManager *tapm, unsigned n)
	: tapm(tapm), n(n)
{
}

void ClientControl::setMainDescriptor(int fd) const
{
	tapm->setMainDescriptor(n, fd);
}

void ClientControl::writeToMain(const vector<uint8_t> &data) const
{
	tapm->writeToMain(n, data);
}

void ClientControl::setTimeout(unsigned timeout) const
{
	const auto wakeup_time = chrono::high_resolution_clock::now() + chrono::seconds(timeout);
	tapm->setTimeout(n, wakeup_time);
}

