
#include <algorithm>
#include "Statistic.h"
#include "Client.h"

using namespace std;

Statistic::Statistic()
	: offline(0), connecting(0), online(0)
{
}

void Statistic::changeState(int from, int to)
{
	switch (from) {
		case Client::OFFLINE:
			offline = max(offline - 1, 0);
			break;
		case Client::CONNECTING:
			connecting = max(connecting - 1, 0);
			break;
		case Client::ONLINE:
			online = max(online - 1, 0);
			break;
		default:
			break;
	}

	switch (to) {
		case Client::OFFLINE:
			offline++;
			break;
		case Client::CONNECTING:
			connecting++;
			break;
		case Client::ONLINE:
			online++;
			break;
		default:
			break;
	}
}
