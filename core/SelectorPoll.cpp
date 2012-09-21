
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "SelectorPoll.h"
#include "Socket.h"

using namespace std;

SelectorPoll::SelectorPoll(int n)
	: fds(n), rcursor(0), wcursor(0)
{
	for (auto &p: fds) {
		p.fd = -1;
		p.events = POLLIN | POLLPRI | POLLOUT;
		p.revents = 0;
	}
}

void SelectorPoll::setSocket(unsigned idx, const shared_ptr<const Socket> &socket)
{
	assert(idx < fds.size());
	fds[idx].fd = socket ? socket->getDescriptor() : -1;
	fds[idx].revents = 0;
}

void SelectorPoll::select()
{
	if (poll(&fds[0], fds.size(), 0) < 0) {
		cerr << "poll failed: " << strerror(errno) << endl;
		for (auto &p: fds) {
			p.revents = 0;
		}
	}

	rcursor = 0;
	wcursor = 0;
}

// TODO: Push clients over callback
int SelectorPoll::selectRead()
{
	while (rcursor < fds.size()) {
		const int i = rcursor++;
		if ((fds[i].revents & (POLLPRI | POLLIN)) != 0) {
			fds[i].revents &= ~(POLLPRI | POLLIN);
			return i;
		}
	}
	
	return -1;
}

// TODO: Push clients over callback
int SelectorPoll::selectWrite(const set<unsigned> &intrest)
{
	if (!intrest.empty()) {
		for (unsigned i = 0; i < fds.size(); i++) {
			if (intrest.count(i) == 0) {
				fds[i].revents &= POLLOUT;
			}
		}
	}
	
	while (wcursor < fds.size()) {
		const int i = wcursor++;
		if ((fds[i].revents & POLLOUT) != 0) {
			fds[i].revents &= POLLOUT;
			return i;
		}
	}
	
	return -1;
}

void SelectorPoll::setDescriptor(unsigned idx, int fd)
{
	assert(idx < fds.size());
	fds[idx].fd = fd;
	fds[idx].revents = 0;
}

int SelectorPoll::getDescriptor(unsigned idx) const
{
	assert(idx < fds.size());
	return fds[idx].fd;
}
