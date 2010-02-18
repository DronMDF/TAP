
#include <string.h>
#include <time.h>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/format.hpp>
#include "Logger.h"

using namespace std;
using namespace boost;

void Logger::logIteration()
{
	static timespec last = {0,0};
	timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	if (last.tv_sec != 0) {
		timespec diff;
		memcpy(&diff, &now, sizeof(timespec));
		if (last.tv_nsec > diff.tv_nsec) {
			diff.tv_nsec += 1000000000;
			diff.tv_sec -= 1;
		}
		
		BOOST_ASSERT(last.tv_sec <= diff.tv_sec);
		
		diff.tv_nsec -= last.tv_nsec;
		diff.tv_sec -= last.tv_sec;

		cout << format("Iteration time: %u.%03us")
			% diff.tv_sec % ((diff.tv_nsec + 500000) / 1000000) << endl;
	}

	memcpy(&last, &now, sizeof(timespec));
}
