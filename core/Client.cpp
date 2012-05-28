
#include <boost/format.hpp>
#include "Client.h"
#include "Tracer.h"

using namespace std;

Tracer Client::null_tracer;

Client::Client()
	: tracer(&null_tracer), state_changer([](int, int) {}), state(OFFLINE)
{
}

Client::~Client()
{
	setState(OFFLINE);
}

bool Client::writeToMain(ClientControl *, const vector<uint8_t> &data)
{
	// TODO: Interrupt safe
	int fd = getMain();
	return write(fd, &data[0], data.size()) == int(data.size());
}

void Client::action(ClientControl *control)
{
}

void Client::setStatsChanger(function<void (int, int)> state_changer)
{
	this->state_changer = state_changer;
}

void Client::setState(int new_state)
{
	state_changer(state, new_state);
	state = new_state;
}

int Client::getState() const
{
	return state;
}

void Client::setTracer(Tracer *tracer)
{
	this->tracer = tracer;
}

