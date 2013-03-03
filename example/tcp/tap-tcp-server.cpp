
#include <algorithm>
#include <chrono>
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
#include <sched.h>
#include <core/Tap.h>
#include <core/SelectorEpoll.h>
#include <core/SocketHandler.h>
#include <core/SocketTcp.h>

using namespace std;
using namespace std::chrono;

class Server {
public:
	Server(unsigned port, unsigned count);

	void listenning();
private:
	void setNonblock(int fd) const;
	bool insertDescriptor(int fd);

	void pollIn(int fd);
	void pollOut(int fd);

	int lsock;
	pollfd cfd;

	vector<pollfd> pfd;
	vector<uint8_t> idata;
	vector<uint8_t> odata;

	uint64_t readed;
	uint64_t writed;

	time_point<high_resolution_clock> outtime;
};

Server::Server(unsigned port, unsigned count)
	: lsock(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)), cfd({0, 0, 0}), pfd(count),
	  idata(4096), odata(4096), readed(0), writed(0), outtime(high_resolution_clock::now())
{
	cfd.fd = lsock;
	cfd.events = POLLIN | POLLPRI;
	cfd.revents = 0;

	for(auto &p: pfd) {
		p.fd = -1;
		p.events = POLLIN | POLLOUT;
		p.revents = 0;
	}

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
		const auto interval = high_resolution_clock::now() - outtime;
		if (interval > seconds(10)) {
			int count = 0;
			for (const auto p: pfd) {
				if (p.fd != -1) {
					count++;
				}
			}

			const auto ms = duration_cast<milliseconds>(interval).count();
			cout << "Clients:" << count << " ";
			cout << "Read: " << (readed * 8000 / ms) << " bit/s; ";
			cout << "Write: " << (writed * 8000 / ms) << " bit/s" << endl;

			readed = writed = 0;
			outtime = high_resolution_clock::now();
		}

		while (poll(&cfd, 1, 0) > 0) {
			const int nfd = accept(lsock, 0, 0);
			if (nfd != -1) {
				// cout << "opening socket " << nfd << endl;
				setNonblock(nfd);
				if (!insertDescriptor(nfd)) {
					close(nfd);
				}
			}
		}

		int pc = poll(&pfd[0], pfd.size(), 0);
		if (pc <= 0) {
			sleep(1);
			continue;
		}

		int pa = 0;
		for(auto &p: pfd) {
			if (pa > pc) {
				break;
			}

			if (p.revents != 0) {
				pa++;
			}

			if ((p.revents & POLLPRI) != 0) {
				pollIn(p.fd);
			}

			if ((p.revents & POLLIN) != 0) {
				pollIn(p.fd);
			}

			if ((p.revents & POLLOUT) != 0) {
				pollOut(p.fd);
			}

			if ((p.revents & (POLLRDHUP | POLLERR | POLLHUP |  POLLNVAL)) != 0) {
				//cout << "closing socket " << p.fd << endl;
				close(p.fd);
				p.fd = -1;
			}
		}
	}
}

void Server::setNonblock(int fd) const
{
	const int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool Server::insertDescriptor(int fd)
{
	for(auto &p: pfd) {
		if (p.fd == -1) {
			p.fd = fd;
			return true;
		}
	}
	return false;
}

void Server::pollIn(int fd)
{
	const int rv = read(fd, &idata[0], idata.size());
	if (rv > 0) {
		readed += rv;
	}
}

void Server::pollOut(int fd)
{
	const int rv = write(fd, &odata[0], odata.size());
	if (rv > 0) {
		writed += rv;
	}
}

class WriteHandler : public SocketHandler {
	static vector<uint8_t> buffer;

	virtual void recv(const vector<uint8_t> &data) override {
		copy(data.begin(), data.end(), buffer.begin());
	}

	virtual vector<uint8_t> send() override {
		return buffer;
	}
};

vector<uint8_t> WriteHandler::buffer(1024 * 64, 'B');

class SocketTcpListen : public SocketTcp {
private:
	Selector *selector;
	SocketTcpListen(const SocketTcpListen &);
	SocketTcpListen &operator =(const SocketTcpListen &);
public:
	SocketTcpListen(Selector *selector)
		: SocketTcp(0), selector(selector)
	{
	}

	virtual void recv() override {
		const int nfd = accept(getDescriptor(), 0, 0);
		if (nfd == -1) {
			throw runtime_error(string("accept failed: ") + strerror(errno));
		}
		auto nsock = make_shared<SocketTcp>(nfd, make_shared<WriteHandler>());
		selector->addSocket(nsock);
	}

	virtual void send() override {
		throw "Invalid method for listening socket";
	}
};

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

	SelectorEpoll selector;

	auto listen_socket = make_shared<SocketTcpListen>(&selector);
	listen_socket->bind(port);

	selector.addSocket(listen_socket);

	while(true) {
		selector.proceed();
	}

	return 0;
}
