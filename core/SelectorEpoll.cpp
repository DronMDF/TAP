
#include "SelectorEpoll.h"
#include <cstring>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>
#include "Socket.h"

using namespace std;

SelectorEpoll::SelectorEpoll(int n)
	: epollfd(epoll_create(n)), sockets()
{
	if (epollfd == -1) {
		throw runtime_error(string("epoll_create failed: ") + strerror(errno));
	}
}

SelectorEpoll::~SelectorEpoll()
{
	close(epollfd);
}

void SelectorEpoll::addSocket(const shared_ptr<Socket> &socket)
{
	const int fd = socket->getDescriptor();
	if (sockets.count(fd) > 0) {
		throw runtime_error("Descriptor already in use");
	}
	sockets[fd] = socket;

	struct epoll_event ev = { EPOLLIN | EPOLLOUT | EPOLLPRI, {}};
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		throw runtime_error(string("epoll_ctl(EPOLL_CTL_ADD) failed: ") + strerror(errno));
	}
}

void SelectorEpoll::removeSocket(const shared_ptr<Socket> &socket)
{
	const int fd = socket->getDescriptor();
	if (sockets.count(fd) > 0) {
		throw runtime_error("Descriptor not in use");
	}
	sockets.erase(fd);
}

void SelectorEpoll::proceed()
{
	vector<epoll_event> evs(sockets.size());
	int count = epoll_wait(epollfd, &evs[0], evs.size(), 0);
	if (count == -1) {
		throw runtime_error(string("epoll_wait() failed: ") + strerror(errno));
	}
	evs.resize(count);

	// Read all
	for (int ec = 0; ec < count; ec++) {
		if ((evs[ec].events & EPOLLIN) != 0) {
			sockets[evs[ec].data.fd]->recv();
			evs[ec].events &= ~EPOLLIN;
		}
	}

	// Write all
	for (int ec = 0; ec < count; ec++) {
		if ((evs[ec].events & EPOLLOUT) != 0) {
			sockets[evs[ec].data.fd]->send();
			evs[ec].events &= ~EPOLLOUT;
		}
	}

	// Handle all errors
	for (int ec = 0; ec < count; ec++) {
		if (evs[ec].events != 0) {
			sockets[evs[ec].data.fd]->recv();
		}
	}
}

