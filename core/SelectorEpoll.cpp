
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "SelectorEpoll.h"
#include "Socket.h"

using namespace std;

SelectorEpoll::SelectorEpoll(int n)
	: epollfd(epoll_create(n)), fds(n), events(n), event_count(0)
{
	if (epollfd == -1) {
		throw runtime_error(string("epoll_create failed") + strerror(errno));
	}

	for (auto &fd: fds) {
		fd = -1;
	}
}

SelectorEpoll::~SelectorEpoll()
{
	close(epollfd);
}

void SelectorEpoll::setSocket(unsigned n, const shared_ptr<const Socket> &socket)
{
	assert(n < fds.size());
	const int nfd = socket->getDescriptor();
	assert(fds[n] != nfd);

	struct epoll_event ev = { EPOLLIN | EPOLLOUT | EPOLLPRI, {}};
	if (fds[n] != -1) {
		ev.data.fd = fds[n];
		if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fds[n], &ev) == -1) {
			cerr << "epoll_ctl(EPOLL_CTL_DEL, " << fds[n] << ") failed. "
				<< strerror(errno) << endl;
			return;
		}

		fds[n] = -1;
	}

	if (nfd != -1) {
		ev.data.fd = nfd;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, nfd, &ev) == -1) {
			cerr << "epoll_ctl(EPOLL_CTL_ADD, " << nfd << ") failed. "
				<< strerror(errno) << endl;
		} else {
			fds[n] = nfd;
		}
	}
}

void SelectorEpoll::select()
{
	event_count = epoll_wait(epollfd, &events[0], events.size(), 0);
	if (event_count == -1) {
		cerr << "epoll_wait failed. " << strerror(errno) << endl;
		event_count = 0;
	}
}

void SelectorEpoll::selectRead(const function<void (int)> &callback)
{
	const int flags = EPOLLPRI | EPOLLIN | EPOLLERR | EPOLLHUP;
	for (int ec = 0; ec < event_count; ec++) {
		if ((events[ec].events & flags) == 0) {
			continue;
		}

		// Not optimal way
		for (unsigned n = 0; n < fds.size(); n++) {
			if (fds[n] == events[ec].data.fd) {
				callback(n);
			}
		}
	}
}

void SelectorEpoll::selectWrite(const function<void (int)> &callback)
{
	const int flags = EPOLLOUT | EPOLLERR | EPOLLHUP;
	for (int ec = 0; ec < event_count; ec++) {
		if ((events[ec].events & flags) == 0) {
			continue;
		}

		// Not optimal way
		for (unsigned n = 0; n < fds.size(); n++) {
			if (fds[n] == events[ec].data.fd) {
				callback(n);
			}
		}
	}
}

void SelectorEpoll::setDescriptor(unsigned, int)
{
}
