
#include "Client.h"

using namespace std;

Client::Client()
	: state_changer([](int, int) {}), state(OFFLINE)
{
}

Client::~Client()
{
	state_changer(state, OFFLINE);
	state = OFFLINE;
}

void Client::setStatsChanger(function<void (int, int)> state_changer)
{
	this->state_changer = state_changer;
}
