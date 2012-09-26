
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "SelectorEpoll.h"
#include "Socket.h"

using namespace std;

SelectorEpoll::SelectorEpoll(int n)
	: fds(n)
{
	for (auto &p: fds) {
		p.fd = -1;
		p.events = POLLIN | POLLPRI | POLLOUT;
		p.revents = 0;
	}
}

void SelectorEpoll::setSocket(unsigned idx, const shared_ptr<const Socket> &socket)
{
	assert(idx < fds.size());
	fds[idx].fd = socket ? socket->getDescriptor() : -1;
	fds[idx].revents = 0;
}

void SelectorEpoll::select()
{
	if (poll(&fds[0], fds.size(), 0) < 0) {
		cerr << "poll failed: " << strerror(errno) << endl;
		for (auto &p: fds) {
			p.revents = 0;
		}
	}
}

void SelectorEpoll::selectRead(const function<void (int)> &callback)
{
	const int flags = POLLPRI | POLLIN | POLLERR | POLLHUP | POLLNVAL;
	for (unsigned i = 0; i < fds.size(); i++) {
		if ((fds[i].revents & flags) != 0) {
			callback(i);
		}
	}
}

void SelectorEpoll::selectWrite(const function<void (int)> &callback)
{
	const int flags = POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	for (unsigned i = 0; i < fds.size(); i++) {
		if ((fds[i].revents & flags) != 0) {
			callback(i);
		}
	}
}

void SelectorEpoll::setDescriptor(unsigned, int)
{
}
