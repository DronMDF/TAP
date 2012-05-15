
#include <assert.h>
#include <boost/foreach.hpp>
#include "SelectorPoll.h"

SelectorPoll::SelectorPoll(int n)
	: fds(n)
{
	BOOST_FOREACH(auto &p, fds) {
		p.fd = -1;
		p.events = POLLIN;
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
