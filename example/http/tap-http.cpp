
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/regex.hpp>
#include <getopt.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/resource.h>
#include <core/SelectorPoll.h>
#include <core/TapManager.h>
#include <core/TracerStream.h>
#include "ClientHttp.h"

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
	cout << "HTTP load utility" << endl;
	cout << "$ tap-as [OPTIONS] URL" << endl;
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
	
	const string url = argv[optind++];
	in_addr server = { inet_addr("127.0.0.1") };
	string file = "/";
	
	// TODO: std::regex is not implemented in gcc 4.6	
	boost::regex re("http://([^/]*)(/.*)");
	boost::smatch what;
	if (boost::regex_match(url, what, re)) {
		const string host(what[1].first, what[1].second);
		if (auto *record = gethostbyname(host.c_str())) {
			server = *reinterpret_cast<in_addr *>(record->h_addr_list[0]);
		}
		
		file.assign(what[2].first, what[2].second);
	}

	cout << "Fetched url: http://" << inet_ntoa(server) << file << endl;
	
	TracerStream tracer(&cout, timestamp_millis);
	
	const string request = "GET " + file + " HTTP/1.0\r\n\r\n";
	TapManager tapm(count,
			[](int n){ return make_shared<SelectorPoll>(n); }, 
			[server, request](){ return make_shared<ClientHttp>(server, 7777, request); });
	
	tapm.setTracer(0, &tracer);
	
	tapm.setShowStatistic(bind(showStatistic, _1, _2, _3, timestamp_millis), chrono::seconds(10));
	
	tapm.pressure();
	return 0;
}
