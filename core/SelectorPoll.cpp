
#include "SelectorPoll.h"

#include <poll.h>
#include "Socket.h"

using namespace std;

SelectorPoll::SelectorPoll()
	: Selector()
{
}

SelectorPoll::SelectorPoll(int)
	: Selector()
{
}

void SelectorPoll::proceed()
{
	vector<pollfd> fds;
	fds.reserve(sockets.size());
	for (const auto &s: sockets) {
		fds.push_back({ s.first, POLLIN | POLLPRI | POLLOUT, 0});
	}

	if (poll(&fds[0], fds.size(), 0) == 0) {
		return;
	}

	// Read all
	for (auto &p: fds) {
		if ((p.revents & POLLIN) != 0) {
			sockets[p.fd]->recv();
			p.revents &= ~POLLIN;
		}
	}

	// Write all
	for (auto &p: fds) {
		if ((p.revents & POLLOUT) != 0) {
			sockets[p.fd]->send();
			p.revents &= ~POLLOUT;
		}
	}

	// Handle all errors
	for (const auto &p: fds) {
		if (p.revents != 0) {
			sockets[p.fd]->recv();
		}
	}

}
