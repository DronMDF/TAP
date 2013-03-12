
#include <boost/format.hpp>
#include "Client.h"
#include "Socket.h"

using namespace std;

Client::Client()
	: control(0, 0)
{
}

Client::~Client()
{
}

void Client::action(ClientControl *)
{
}

void Client::setClientControl(const ClientControl &c)
{
	control = c;
}
