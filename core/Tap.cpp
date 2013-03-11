
#include <stdexcept>
#include <signal.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

using namespace std;

void tap_init(unsigned)
{
	signal(SIGPIPE, SIG_IGN);

	const unsigned need_fdcount = 100000;

	rlimit nofile;
	if (getrlimit(RLIMIT_NOFILE, &nofile) == 0 and
		(nofile.rlim_cur < need_fdcount or nofile.rlim_max < need_fdcount))
	{
		nofile.rlim_cur = need_fdcount;
		nofile.rlim_max = need_fdcount;

		if (getuid() != 0) {
			//throw runtime_error("root need for change RLIMIT_NOFILE");
			return;
		}

		if (setrlimit(RLIMIT_NOFILE, &nofile) == -1) {
			throw runtime_error(string("Error change RLIMIT_NOFILE: ") + strerror(errno));
		}
	}
}
