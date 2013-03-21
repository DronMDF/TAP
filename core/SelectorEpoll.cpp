
#include "SelectorEpoll.h"
#include <cstring>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>
#include "Socket.h"

using namespace std;

SelectorEpoll::SelectorEpoll()
	: epollfd(epoll_create(1))
{
	if (epollfd == -1) {
		throw runtime_error(string("epoll_create failed: ") + strerror(errno));
	}
}

SelectorEpoll::~SelectorEpoll() noexcept
{
	close(epollfd);
}

void SelectorEpoll::addSocket(const shared_ptr<Socket> &socket)
{
	const int fd = socket->getDescriptor();
	epoll_event ev = { EPOLLIN | EPOLLOUT | EPOLLPRI, {}};
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		throw runtime_error(string("epoll_ctl(EPOLL_CTL_ADD) failed: ") + strerror(errno));
	}

	Selector::addSocket(socket);
}

void SelectorEpoll::removeSocket(const shared_ptr<Socket> &socket)
{
	if (epoll_ctl(epollfd, EPOLL_CTL_DEL, socket->getDescriptor(), NULL) == -1) {
		throw runtime_error(string("epoll_ctl(EPOLL_CTL_DEL) failed: ") + strerror(errno));
	}

	Selector::removeSocket(socket);
}

int SelectorEpoll::recv(unsigned enable, int fd, int) const
{
	if (enable != 0) {
		const auto s = sockets.find(fd);
		if (s != sockets.end()) {
			return s->second->recv() ? 0 : -1;
		}
	}
	return 0;
}

int SelectorEpoll::send(unsigned enable, int fd, int) const
{
	if (enable != 0) {
		const auto s = sockets.find(fd);
		if (s != sockets.end()) {
			return s->second->send() ? 0 : -1;
		}
	}
	return 0;
}

void SelectorEpoll::strategy(const vector<epoll_event> &evs)
{
	// This is stupid strategy... override this method if you want change strategy.
	// We cannot live socket untouched. All event should be handled
	// For timeboxing we can reduce evs size (its simple)
	// Or reduce read/write data size (need pass rate to socket level)

	for (auto &ev: evs) {
		int revents = ev.events;

		if (recv(revents & EPOLLIN, ev.data.fd) == -1) {
			revents |= EPOLLERR;
		}

		if (send(revents & EPOLLOUT, ev.data.fd) == -1) {
			revents |= EPOLLERR;
		}

		if (recv(revents & ~(EPOLLIN | EPOLLOUT), ev.data.fd) == -1) {
			removeSocket(sockets[ev.data.fd]);
		}
	}
}

void SelectorEpoll::proceed()
{
	vector<epoll_event> evs(sockets.size());
	if (evs.empty()) {
		return;
	}

	int count = epoll_wait(epollfd, &evs[0], evs.size(), 0);
	if (count == -1) {
		throw runtime_error(string("epoll_wait() failed: ") + strerror(errno));
	}

	evs.resize(count);
	strategy(evs);
}

