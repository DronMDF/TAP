
#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "SelectorPoll.h"

using namespace std;

SelectorPoll::SelectorPoll(int n)
	: fds(n), read_cursor(0)
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

int SelectorPoll::select()
{
	// TODO: Возвращаем дескрипторы без системного вызова, если они в нужном состоянии
	// TODO: различать готовность на чтение и на запись (два разных вызова?)
	int rv = poll(&fds[0], fds.size(), 0);
	if (rv > 0) {
		for (unsigned i = 0; i < fds.size(); i++) {
			if (fds[i].revents != 0) {
				fds[i].revents = 0;
				return i;
			}
		}
	}
	
	return -1;
}

int SelectorPoll::findActual(int status)
{
	while (read_cursor < fds.size()) {
		const int i = read_cursor++;
		if (fds[i].fd == -1) {
			return i;
		}
		
		if ((fds[i].revents & (status | POLLERR | POLLHUP)) != 0) {
			fds[i].revents &= ~status;
			return i;
		}
	}
	
	return -1;
}

int SelectorPoll::selectRead()
{
	if (read_cursor < fds.size()) {
		int idx = findActual(POLLIN);
		if (idx < int(fds.size())) {
			return idx;
		}
		
		// Дошли до конца набора - выйдет чтобы была возможность отработать райт
		return -1;
	}
	
	read_cursor = 0;
	int rv = poll(&fds[0], fds.size(), 0);
	if (rv < 0) {
		cerr << "poll failed: " << strerror(errno) << endl;
		throw runtime_error("Ошибка при выполнении poll");
	}

	if (rv > 0) {
		int idx = findActual(POLLIN);
		if (idx < int(fds.size())) {
			return idx;
		}
	}

	return -1;
}
