
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
