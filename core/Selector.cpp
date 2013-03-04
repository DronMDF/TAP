
#include "Selector.h"

#include <stdexcept>
#include "Socket.h"

using namespace std;

Selector::Selector()
	: sockets()
{
}

Selector::~Selector()
{
}

void Selector::addSocket(const std::shared_ptr<Socket> &socket)
{
	const int fd = socket->getDescriptor();
	if (sockets.find(fd) != sockets.end()) {
		throw runtime_error("Descriptor already in use");
	}
	sockets[fd] = socket;
}

void Selector::removeSocket(const std::shared_ptr<Socket> &socket)
{
	const int fd = socket->getDescriptor();
	if (sockets.find(fd) == sockets.end()) {
		throw runtime_error("Descriptor not in use");
	}
	sockets.erase(fd);
}

size_t Selector::getCount() const
{
	return sockets.size();
}
