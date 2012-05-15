
#include <string>
#include <core/SelectorPoll.h>
#include <core/TapManager.h>
#include <http_client/ClientBuilderHttp.h>

using namespace std;

int main(int /*argc*/, const char **/*argv*/)
{
	TapManager tapm(1000, [](int n){ return make_shared<SelectorPoll>(n); }, 
			ClientBuilderHttp(string("10.4.2.91"), 80, string("server request")));
	tapm.pressure();
	return 0;
}
