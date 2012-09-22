
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

void SelectorPoll::selectRead(const function<void (int)> &callback)
{
	const int flags = POLLPRI | POLLIN | POLLERR | POLLHUP | POLLNVAL;
	for (unsigned i = 0; i < fds.size(); i++) {
		if ((fds[i].revents & flags) != 0) {
			callback(i);
		}
	}
}

void SelectorPoll::selectWriteCallback(const function<void (int)> &callback)
{
	const int flags = POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	for (unsigned i = 0; i < fds.size(); i++) {
		if ((fds[i].revents & flags) != 0) {
			callback(i);
		}
	}
}

int SelectorPoll::selectRead()
{
	const int flags = POLLPRI | POLLIN | POLLERR | POLLHUP | POLLNVAL;
	while (rcursor < fds.size()) {
		const int i = rcursor++;
		if ((fds[i].revents & flags) != 0) {
			fds[i].revents &= ~flags;
			return i;
		}
	}
	
	return -1;
}

int SelectorPoll::selectWrite(const set<unsigned> &intrest)
{
	const int flags = POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	if (!intrest.empty()) {
		for (unsigned i = 0; i < fds.size(); i++) {
			if (intrest.count(i) == 0) {
				fds[i].revents &= ~POLLOUT;
			}
		}
	}
	
	while (wcursor < fds.size()) {
		const int i = wcursor++;
		if ((fds[i].revents & flags) != 0) {
			fds[i].revents &= ~flags;
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
