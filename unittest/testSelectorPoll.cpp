
#include <memory.h>
#include <boost/test/unit_test.hpp>
#include <core/SelectorPoll.h>
#include <core/Socket.h>
#include "SocketTest.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteSelectorPoll);

struct piper {
	int in;
	int out;
	piper() : in(-1), out(-1) {
		int fd[2];
		if (pipe(fd) == 0) {
			in = fd[0];
			out = fd[1];
		}
	}
	~piper() {
		close(in);
		close(out);
	}
};

// TODO: Implement add/proceed/remove tests

BOOST_AUTO_TEST_SUITE_END();
