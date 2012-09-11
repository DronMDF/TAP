
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <boost/foreach.hpp>
#include <arpa/inet.h>
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <core/Tap.h>

using namespace std;

class Server {
public:
	Server(unsigned port, unsigned count);

	void listenning();
private:
	void setNonblock(int fd) const;
	bool insertDescriptor(int fd);

	int lsock;
	vector<pollfd> pfd;
	vector<uint8_t> idata;
	vector<uint8_t> odata;
};

void Server::setNonblock(int fd) const
{
	const int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool Server::insertDescriptor(int fd)
{
	BOOST_FOREACH(pollfd &p, pfd) {
		if (p.fd == -1) {
			p.fd = fd;
			return true;
		}
	}
	return false;
}

Server::Server(unsigned port, unsigned count)
	: lsock(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)), pfd(count + 1), idata(4096),
	  odata(4096)
{
	BOOST_FOREACH(pollfd &p, pfd) {
		p.fd = -1;
		p.events = POLLIN | POLLOUT;
		p.revents = 0;
	}

	pfd[0].fd = lsock;
	pfd[0].events = POLLIN | POLLPRI;

	generate(odata.begin(), odata.end(), rand);

	if (lsock == -1) {
		throw runtime_error(string("Cannot create socket") + strerror(errno));
	}

	const int reuse = 1;
	setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	setNonblock(lsock);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (::bind(lsock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
		close(lsock);
		throw runtime_error(string("Cannot bind socket") + strerror(errno));
	}

	if (listen(lsock, count) == -1) {
		close(lsock);
		throw runtime_error(string("Cannot listen socket") + strerror(errno));
	}
}

void Server::listenning()
{
	while (true) {
		int rv = ::poll(&pfd[0], pfd.size(), 0);
		if (rv < 1) {
			sched_yield();
			continue;
		}

		BOOST_FOREACH(pollfd &p, pfd) {
			if (p.revents == 0) {
				continue;
			}

			if (p.fd == lsock and (p.revents & POLLIN) != 0) {
				// ListenSocket
				const int nfd = accept(lsock, 0, 0);
				if (nfd != -1) {
					cout << "opening socket " << nfd << endl;
					setNonblock(nfd);
					if (!insertDescriptor(nfd)) {
						close(nfd);
					}
				}
				continue;
			}

			if ((p.revents & POLLIN) != 0) {
				read(p.fd, &idata[0], idata.size());
			}

			if ((p.revents & POLLOUT) != 0) {
				write(p.fd, &odata[0], odata.size());
			}

			if ((p.revents & (POLLRDHUP | POLLERR | POLLHUP |  POLLNVAL)) != 0) {
				cout << "closing socket " << p.fd << endl;
				close(p.fd);
				p.fd = -1;
			}
		}
	}
}

void usage()
{
	cout << "TAP TCP server" << endl;
	cout << "$ tap-tcp-server [OPTIONS]" << endl;
	cout << "Options:" << endl;
	cout << "  -n, --count NUM\t\tquantity of clients" << endl;
	cout << "  -p, --port NUM\t\tlisten port" << endl;
	cout << "  -h, -?, --help\t\tthis help" << endl;
}

int main(int argc, char **argv)
{
	srand(time(0));

	unsigned count = 1000;
	unsigned port = 6666;

	struct option long_options[] = {
		{ "count", required_argument, 0, 'n' },
		{ "port", no_argument, 0, 'p' },
		{ "help", no_argument, 0, 'h' },
		{ 0, 0, 0, 0 }
	};

	while (true) {
		const auto c = getopt_long(argc, argv, "n:p:h?", long_options, 0);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'n':
			count = atoi(optarg);
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'h':
		case '?':
		default:
			usage();
			exit(0);
		}
	}

	cout << "Clients count: " << count << endl;
	tap_init(count);

	Server server(port, count);
	server.listenning();

	return 0;
}
