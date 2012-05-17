
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "SelectorPoll.h"

using namespace std;

SelectorPoll::SelectorPoll(int n)
	: fds(n), read_cursor(-1), write_cursor(-1)
{
	BOOST_FOREACH(auto &p, fds) {
		p.fd = -1;
		p.events = POLLIN | POLLOUT;
		p.revents = 0;
	}
}

void SelectorPoll::setDescriptor(unsigned idx, int fd)
{
	assert(idx < fds.size());
	fds[idx].fd = fd;
}

int SelectorPoll::getDescriptor(unsigned idx) const
{
	assert(idx < fds.size());
	return fds[idx].fd;
}

int SelectorPoll::findActual(unsigned cursor, int status)
{
	while (cursor < fds.size()) {
		if (fds[cursor].fd == -1) {
			return cursor;
		}
		
		if ((fds[cursor].revents & (status | POLLERR | POLLHUP | POLLNVAL)) != 0) {
			// TODO: Это не понадобиьтся, когда мы будем сканировать на write
			fds[cursor].revents &= ~status;
			return cursor;
		}
		
		cursor++;
	}
	
	return -1;
}

int SelectorPoll::selectRead()
{
	if (read_cursor == int(fds.size())) {
		read_cursor = -1;
		return -1;
	}
	
	if (read_cursor >= 0) {
		int idx = findActual(read_cursor, POLLIN);
		if (idx >= 0) {
			read_cursor = idx + 1;
			return idx;
		}

		// Дошли до конца набора - выйдем чтобы была возможность отработать райт
		read_cursor = -1;
		return -1;
	}
	
	int rv = poll(&fds[0], fds.size(), 0);
	if (rv < 0) {
		cerr << "poll failed: " << strerror(errno) << endl;
		throw runtime_error("Ошибка при выполнении poll");
	}

	if (rv > 0) {
		int idx = findActual(0, POLLIN);
		if (idx >= 0) {
			read_cursor = idx + 1;
			return idx;
		}
		
		read_cursor = -1;
	}

	return -1;
}

int SelectorPoll::selectWrite()
{
	if (write_cursor == int(fds.size())) {
		write_cursor = -1;
		return -1;
	}
	
	if (write_cursor >= 0) {
		int idx = findActual(write_cursor, POLLOUT);
		if (idx >= 0) {
			write_cursor = idx + 1;
			return idx;
		}
		
		write_cursor = -1;
		return -1;
	}
	
	int rv = poll(&fds[0], fds.size(), 0);
	if (rv < 0) {
		cerr << "poll failed: " << strerror(errno) << endl;
		throw runtime_error("Ошибка при выполнении poll");
	}

	if (rv > 0) {
		int idx = findActual(0, POLLOUT);
		if (idx >= 0) {
			write_cursor = idx + 1;
			return idx;
		}
		
		write_cursor = -1;
	}

	return -1;
}
