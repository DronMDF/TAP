
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <getopt.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/resource.h>
#include <core/SelectorPoll.h>
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
	out << "] ";
	
	return out.str();
}

string timestamp_millis()
{
	using namespace chrono;
	const auto time = high_resolution_clock::now();
	ostringstream out;
	out << duration_cast<milliseconds>(time.time_since_epoch()).count() << " ";
	return out.str();
}

void showStatistic(int offline, int connecting, int online, function<string ()> timestamp)
{
	cout << timestamp() 
		<< "offline: " << offline
		<< " connecting: " << connecting
		<< " online: " << online
		<< endl;
}

void init(unsigned n)
{
	signal(SIGPIPE, SIG_IGN);
	
	const unsigned need_fdcount = n * 3;
	
	rlimit nofile;
	if (getrlimit(RLIMIT_NOFILE, &nofile) == 0 and 
		(nofile.rlim_cur < need_fdcount or nofile.rlim_max < need_fdcount))
	{
		nofile.rlim_cur = need_fdcount;
		nofile.rlim_max = need_fdcount;
	
		if (getuid() != 0) {
			cerr << "root need for change RLIMIT_NOFILE" << endl;
			exit(-1);
		}
		
		if (setrlimit(RLIMIT_NOFILE, &nofile) == -1) {
			cerr << "Error change RLIMIT_NOFILE: " << strerror(errno) << endl;
			exit(-1);
		}
	}
}

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
	init(count);	
	
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
	
	TracerStream tracer(&cout, timestamp_millis);
	
	TapManager tapm(count,
			[](int n){ return make_shared<SelectorPoll>(n); }, 
			[server, port](){ return make_shared<ClientTcp>(server, port); });
	
	for (unsigned i = 0; i < count; i++) {
		tapm.setTracer(i, &tracer);
	}
	
	tapm.setShowStatistic(bind(showStatistic, _1, _2, _3, timestamp_millis), chrono::seconds(10));
	tapm.pressure();
	
	return 0;
}
