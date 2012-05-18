
#include <signal.h>
#include <string.h>
#include <sys/resource.h>
#include <iostream>
#include <string>
#include <core/SelectorPoll.h>
#include <core/TapManager.h>
#include <core/TracerStream.h>
#include <http_client/ClientHttp.h>

using namespace std;

int main(int /*argc*/, const char **/*argv*/)
{
	signal(SIGPIPE, SIG_IGN);
	
	rlimit nofile = { 100000, 100000 };
	if (setrlimit(RLIMIT_NOFILE, &nofile) == -1) {
		cerr << "Ошибка установки RLIMIT_NOFILE: " << strerror(errno) << endl;
	}
		
	//in_addr server = { inet_addr("10.4.2.91") };
	//string request = "GET hgwebdir.cgi/tap/archive/tip.tar.bz2 HTTP/1.0\r\n\r\n";
	//string request = "GET porn HTTP/1.0\r\n\r\n";

	in_addr server = { inet_addr("127.0.0.1") };
	string request = "GET /garbage.bin HTTP/1.0\n\r\n\r";

	TracerStream tracer(&cout);
	
	TapManager tapm(1000,
			[](int n){ return make_shared<SelectorPoll>(n); }, 
			[server, request](){ return make_shared<ClientHttp>(server, 80, request); });
	
	tapm.setTracer(0, &tracer);
	
	tapm.pressure();
	return 0;
}
