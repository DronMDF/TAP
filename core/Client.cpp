
#include "Client.h"

using namespace std;

Client::Client()
	: state_changer([](int, int) {}), state(OFFLINE)
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
