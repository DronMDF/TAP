
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <getopt.h>
#include <netdb.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <core/SelectorEpoll.h>
#include <core/Tap.h>
#include <core/TapManager.h>
#include <core/TracerStream.h>
#include "ClientTcp.h"

using namespace std;
using namespace std::placeholders;

string timestamp()
{
	using namespace chrono;
	auto time = high_resolution_clock::now();

	ostringstream out;
	out << "[";
	
	// TODO: std::put_time not implemented in gcc 4.6
	auto timesec = system_clock::to_time_t(time);
	char timestr[100];
	if (strftime(timestr, 100, "%T", localtime(&timesec))) {
		out << timestr;
	} else {
		out << timesec;
	}

	auto millis = duration_cast<milliseconds>(time.time_since_epoch()).count() % 1000;
	out << "." << setw(3) << setfill('0') << millis;
	out << "]";
	
	return out.str();
}

string timestamp_millis()
{
	using namespace chrono;
	const auto time = high_resolution_clock::now();
	return boost::lexical_cast<string>(duration_cast<milliseconds>(time.time_since_epoch()).count());
}

void showStatistic(int offline, int connecting, int online, function<string ()> timestamp)
{
	cout << timestamp() 
		<< " offline: " << offline
		<< " connecting: " << connecting
		<< " online: " << online
		<< endl;
}

class SelectorWaiting : public SelectorEpoll {
private:
	const int n;
	set<int> active;
public:
	SelectorWaiting(int n) : n(n), active() {}

	virtual void strategy(const vector<epoll_event> &evs) override {
		const int size = (active.size() < size_t(n)) ? 1 : 65536;
		for (auto &ev: evs) {
			const int rv = recv(ev.events, ev.data.fd, size);
			if (rv == -1) {
				active.erase(ev.data.fd);
				removeSocket(sockets[ev.data.fd]);
			} else if (rv > 0){
				active.insert(ev.data.fd);
			}
		}
	}
};

void usage()
{
	cout << "TCP load utility" << endl;
	cout << "$ tap-tcp [OPTIONS] ADDR:PORT" << endl;
	cout << "Options:" << endl;
	cout << "  -n, --count NUM\t\tquantity of clients" << endl;
	cout << "  -h, -?, --help\t\tthis help" << endl;
}

int main(int argc, char **argv)
{
	unsigned count = 1000;
	
	struct option long_options[] = {
		{ "count", required_argument, 0, 'n' },
		{ "help", no_argument, 0, 'h' },
		{ 0, 0, 0, 0 }
	};

	while (true) {
		const auto c = getopt_long(argc, argv, "n:h?", long_options, 0);
		if (c == -1) {
			break;
		}
		
		switch (c) {
		case 'n':
			count = atoi(optarg);
			break;
		case 'h':
		case '?':
		default:
			usage();
			exit(0);
		}
	}
	
	if (optind >= argc) {
		usage();
		exit(0);
	}
	
	cout << "Clients count: " << count << endl;
	tap_init(count);
	
	const string fetch_point = argv[optind++];
	in_addr server = { inet_addr("127.0.0.1") };
	unsigned port = 6666;
	
	// TODO: std::regex is not implemented in gcc 4.6	
	boost::regex re("(.*):(\\d*)");
	boost::smatch what;
	if (boost::regex_match(fetch_point, what, re)) {
		const string host(what[1].first, what[1].second);
		if (auto *record = gethostbyname(host.c_str())) {
			server = *reinterpret_cast<in_addr *>(record->h_addr_list[0]);
		}

		const string port_str(what[2].first, what[2].second);
		port = boost::lexical_cast<unsigned>(port_str);
	}

	cout << "Server: " << inet_ntoa(server) << ':' << port << endl;
	
	TracerStream tracer(&cout, timestamp);
	
	TapManager tapm(count,
			[](int){ return make_shared<SelectorEpoll>(); },
			[server, port](){ return make_shared<ClientTcp>(server, port); });
	
	for (unsigned i = 0; i < count; i += 1000) {
		tapm.setTracer(i, &tracer);
	}

	tapm.setShowStatistic(bind(showStatistic, _1, _2, _3, timestamp), chrono::seconds(10));
	tapm.pressure();
	
	return 0;
}
