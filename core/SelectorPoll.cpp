
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "SelectorPoll.h"
#include "Socket.h"

using namespace std;

SelectorPoll::SelectorPoll(int n)
	: rfds(n), rcursor(n), wfds(n), wcursor(n)
{
	for (int i = 0; i < n; i++) {
		rfds[i].fd = -1;
		rfds[i].events = POLLIN;
		rfds[i].revents = 0;
		
		wfds[i].fd = -1;
		wfds[i].events = POLLOUT;
		wfds[i].revents = 0;
	}
}

void SelectorPoll::setDescriptor(unsigned idx, int fd)
{
	assert(idx < rfds.size());
	rfds[idx].fd = fd;
}

void SelectorPoll::setSocket(unsigned idx, const shared_ptr<const Socket> &socket)
{
	assert(idx < rfds.size());
	rfds[idx].fd = socket ? socket->getDescriptor() : -1;
}

int SelectorPoll::getDescriptor(unsigned idx) const
{
	assert(idx < rfds.size());
	return rfds[idx].fd;
}

int SelectorPoll::selectRead()
{
	while (rcursor < rfds.size()) {
		const int i = rcursor++;
		if (rfds[i].fd != -1 && rfds[i].revents != 0) {
			rfds[i].revents = 0;
			return i;
		}
	}
	
	int rv = poll(&rfds[0], rfds.size(), 0);
	if (rv < 0) {
		cerr << "poll failed: " << strerror(errno) << endl;
		for (unsigned i = 0; i < rfds.size(); i++) {
			rfds[i].revents = 0;
		}
	}
	
	if (rv > 0) {
		rcursor = 0;
	}

	return -1;
}

int SelectorPoll::selectWrite(const set<unsigned> &intrest)
{
	if (!intrest.empty()) {
		BOOST_FOREACH(auto &p, wfds) {
			p.fd = -1;
		}
		BOOST_FOREACH(auto &i, intrest) {
			wfds[i].fd = rfds[i].fd;
		}

		int rv = poll(&wfds[0], wfds.size(), 0);
		if (rv < 0) {
			cerr << "poll failed: " << strerror(errno) << endl;
			for (unsigned i = 0; i < wfds.size(); i++) {
				wfds[i].revents = 0;
			}
			
			return -1;
		}
		
		if (rv > 0) {
			wcursor = 0;
		}
	}
	
	while (wcursor < wfds.size()) {
		const int i = wcursor++;
		if (wfds[i].revents != 0) {
			wfds[i].revents = 0;
			return i;
		}
	}
	
	return -1;
}

