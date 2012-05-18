
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "SelectorPoll.h"

using namespace std;

SelectorPoll::SelectorPoll(int n)
	: rfds(n)
{
	for (int i = 0; i < n; i++) {
		rfds[i].fd = -1;
		rfds[i].events = POLLIN;
		rfds[i].revents = 0;
	}
}

void SelectorPoll::setDescriptor(unsigned idx, int fd)
{
	assert(idx < rfds.size());
	rfds[idx].fd = fd;
}

int SelectorPoll::getDescriptor(unsigned idx) const
{
	assert(idx < rfds.size());
	return rfds[idx].fd;
}

int SelectorPoll::selectRead()
{
	for (unsigned i = 0; i < rfds.size(); i++) {
		if (rfds[i].revents != 0) {
			rfds[i].revents = 0;
			return i;
		}
	}
	
	int rv = poll(&rfds[0], rfds.size(), 0);
	//cout << "read poll " << rv << endl;
	if (rv < 0) {
		cerr << "poll failed: " << strerror(errno) << endl;
		for (unsigned i = 0; i < rfds.size(); i++) {
			rfds[i].revents = 0;
		}
	}
	
	return -1;
}

bool SelectorPoll::selectWrite(unsigned i)
{
	pollfd wfds = { rfds[i].fd, POLLOUT, 0 };
	int rv = poll(&wfds, 1, 0);
	return rv > 0 and wfds.revents != 0;
}
