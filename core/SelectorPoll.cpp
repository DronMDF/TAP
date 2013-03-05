
#include "SelectorPoll.h"

#include <poll.h>
#include "Socket.h"

using namespace std;

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
			if (sockets[p.fd]->recv()) {
				p.revents &= ~POLLIN;
			} else {
				removeSocket(sockets[p.fd]);
				p.revents = 0;
			}
		}
	}

	// Write all
	for (auto &p: fds) {
		if ((p.revents & POLLOUT) != 0) {
			if (sockets[p.fd]->send()) {
				p.revents &= ~POLLOUT;
			} else {
				removeSocket(sockets[p.fd]);
				p.revents = 0;
			}
		}
	}

	// Handle all errors
	for (const auto &p: fds) {
		if (p.revents != 0) {
			if (!sockets[p.fd]->recv()) {
				removeSocket(sockets[p.fd]);
			}
		}
	}
}
