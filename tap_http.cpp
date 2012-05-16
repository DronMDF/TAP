
#include <string>
#include <core/SelectorPoll.h>
#include <core/TapManager.h>
#include <http_client/ClientHttp.h>

using namespace std;

int main(int /*argc*/, const char **/*argv*/)
{
	in_addr server = { inet_addr("10.4.2.91") };
	string request = "GET hgwebdir.cgi/tap/archive/tip.tar.bz2 HTTP/1.0\r\n\r\n";
	
	TapManager tapm(1000,
			[](int n){ return make_shared<SelectorPoll>(n); }, 
			[server, request](){ return make_shared<ClientHttp>(server, 80, request); });
	tapm.pressure();
	return 0;
}
