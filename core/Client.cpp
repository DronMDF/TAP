
#include <boost/format.hpp>
#include "Client.h"

using namespace std;

Client::Client()
	: queue()
{
}

Client::~Client()
{
}

bool Client::write(ClientControl *, const vector<uint8_t> &data)
{
	// TODO: Interrupt safe
	int fd = getMain();
	return ::write(fd, &data[0], data.size()) == int(data.size());
}

void Client::action(ClientControl *)
{
}

bool Client::wantsToWrite() const
{
	return !queue.empty();
}

void Client::writeToQueue(const vector<uint8_t> &data)
{
	queue.push_back(data);
}
