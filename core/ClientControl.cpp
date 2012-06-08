
#include "ClientControl.h"

#include <chrono>
#include "TapManager.h"
#include "Tracer.h"

using namespace std;

ClientControl::ClientControl(TapManager *tapm, unsigned n, Tracer *tracer)
	: tapm(tapm), n(n), tracer(tracer)
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

void ClientControl::setWakeupTime(const time_point &wakeup_time) const
{
	tapm->setTimeout(n, wakeup_time);
}

void ClientControl::trace(const string &message) const
{
	tracer->trace(message);
}

