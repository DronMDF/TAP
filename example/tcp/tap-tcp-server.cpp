
#include <chrono>
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>
#include <getopt.h>
#include <core/Tap.h>
#include <core/SelectorEpoll.h>
#include <core/SocketHandler.h>
#include <core/SocketTcp.h>

using namespace std;
using namespace std::chrono;

static uint64_t received = 0;
static uint64_t sended = 0;

class WriteHandler : public SocketHandler {
	static vector<uint8_t> buffer;

	virtual void recv(const vector<uint8_t> &data) override {
		copy(data.begin(), data.end(), buffer.begin());
		received += data.size();
	}

	virtual vector<uint8_t> send() override {
		sended += buffer.size();
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

	virtual int recv(int) override {
		const int nfd = accept(getDescriptor(), 0, 0);
		if (nfd == -1) {
			throw runtime_error(string("accept failed: ") + strerror(errno));
		}
		auto nsock = make_shared<SocketTcp>(nfd, make_shared<WriteHandler>());
		selector->addSocket(nsock);
		return 0;
	}

	virtual int send(int) override {
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

	auto outtime = high_resolution_clock::now();

	while(true) {
		selector.proceed();

		const auto interval = high_resolution_clock::now() - outtime;
		if (interval > seconds(10)) {
			const int count = selector.getCount();
			const auto ms = duration_cast<milliseconds>(interval).count();

			cout << "Clients:" << count << " ";
			cout << "Read: " << (received * 8000 / ms) << " bit/s; ";
			cout << "Write: " << (sended * 8000 / ms) << " bit/s" << endl;

			received = sended = 0;
			outtime = high_resolution_clock::now();
		}
	}

	return 0;
}
