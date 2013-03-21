
#include <core/SelectorPoll.h>

#include <memory.h>
#include <boost/test/unit_test.hpp>
#include <core/Socket.h>
#include "SocketTest.h"
#include "TapTestCase.h"

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

TAP_TEST_CASE(testShouldUseAddedSocketInProceed)
{
	// Given
	piper pfd;
	BOOST_REQUIRE_EQUAL(write(pfd.out, "X", 1), 1);

	struct SocketForRead : public SocketTest {
		bool received;
		SocketForRead(int fd) : SocketTest(fd), received(false) {}
		virtual int recv(int) override { received = true; return 0; }
	};
	auto socket = make_shared<SocketForRead>(pfd.in);

	SelectorPoll selector;
	selector.addSocket(socket);
	// When
	selector.proceed();
	// Then
	BOOST_REQUIRE(socket->received);
}

TAP_TEST_CASE(testShouldUseOutputSocketInProceed)
{
	// Given
	struct SocketForWrite : public SocketTest {
		bool sended;
		SocketForWrite(int fd) : SocketTest(fd), sended(false) {}
		virtual int send(int) override { sended = true; return 0; }
	};
	auto socket = make_shared<SocketForWrite>(1);

	SelectorPoll selector;
	selector.addSocket(socket);
	// When
	selector.proceed();
	// Then
	BOOST_REQUIRE(socket->sended);
}

TAP_TEST_CASE(testShouldNotUseRemovedSocketInProceed)
{
	// Given
	struct SocketForWrite : public SocketTest {
		bool sended;
		SocketForWrite(int fd) : SocketTest(fd), sended(false) {}
		virtual int send(int) override { sended = true; return 0; }
	};
	auto socket = make_shared<SocketForWrite>(1);

	SelectorPoll selector;
	selector.addSocket(socket);
	selector.removeSocket(socket);
	// When
	selector.proceed();
	// Then
	BOOST_REQUIRE(!socket->sended);
}

BOOST_AUTO_TEST_SUITE_END();
