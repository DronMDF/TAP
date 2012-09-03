
#include <boost/format.hpp>
#include "Client.h"

using namespace std;

Client::Client()
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

