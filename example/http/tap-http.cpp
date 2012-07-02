
#include <signal.h>
#include <string.h>
#include <sys/resource.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <core/SelectorPoll.h>
#include <core/TapManager.h>
#include <core/TracerStream.h>
#include <http-client/ClientHttp.h>

using namespace std;

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

int main(int /*argc*/, const char **/*argv*/)
{
	signal(SIGPIPE, SIG_IGN);
	
	rlimit nofile = { 100000, 100000 };
	if (setrlimit(RLIMIT_NOFILE, &nofile) == -1) {
		cerr << "Ошибка установки RLIMIT_NOFILE: " << strerror(errno) << endl;
	}
		
	in_addr server = { inet_addr("127.0.0.1") };
	string request = "GET /garbage.bin HTTP/1.0\n\r\n\r";

	TracerStream tracer(&cout, timestamp);
	
	TapManager tapm(1000,
			[](int n){ return make_shared<SelectorPoll>(n); }, 
			[server, request](){ return make_shared<ClientHttp>(server, 80, request); });
	
	tapm.setTracer(0, &tracer);
	
	tapm.pressure();
	return 0;
}
