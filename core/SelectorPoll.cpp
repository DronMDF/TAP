
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "SelectorPoll.h"
#include "Socket.h"

using namespace std;

SelectorPoll::SelectorPoll(int n)
	: fds(n)
{
	for (auto &p: fds) {
		p.fd = -1;
		p.events = POLLIN | POLLPRI | POLLOUT;
		p.revents = 0;
	}
}

void SelectorPoll::addSocket(const shared_ptr<Socket> &)
{
	// TODO: implement this for new API
}

void SelectorPoll::proceed()
{
	// TODO: implement this for new API
}

