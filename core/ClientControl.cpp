
#include "ClientControl.h"

#include <chrono>
#include "TapManager.h"
#include "Tracer.h"

using namespace std;

ClientControl::ClientControl(TapManager *tapm, unsigned n, Tracer *tracer)
	: tapm(tapm), n(n), tracer(tracer)
{
}

void ClientControl::addSocket(const std::shared_ptr<Socket> &socket) const
{
	tapm->setSocket(socket);
}

void ClientControl::removeSocket(const std::shared_ptr<Socket> &socket) const
{
	tapm->removeSocket(socket);
}

void ClientControl::writeToMain(const vector<uint8_t> &data) const
{
	tapm->writeToMain(n, data);
}

void ClientControl::setWakeupTime(const time_point &wakeup_time) const
{
	tapm->setTimeout(n, wakeup_time);
}

void ClientControl::setStateOffline() const
{
	tapm->setStateOffline(n);
}

void ClientControl::setStateConnecting() const
{
	tapm->setStateConnecting(n);
}

void ClientControl::setStateOnline() const
{
	tapm->setStateOnline(n);
}

void ClientControl::trace(const string &message) const
{
	tracer->trace(n, message);
}

void ClientControl::trace(const string &key, unsigned value) const
{
	tracer->trace(n, key, value);
}

