
#include <memory.h>
#include <boost/test/unit_test.hpp>
#include <core/SelectorEpoll.h>
#include <core/Socket.h>
#include "SocketTest.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteSelectorEpoll);

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

struct SocketFlagged : public SocketTest {
	bool received, sended;
	SocketFlagged(int fd) : SocketTest(fd), received(false), sended(false) {};
	virtual void recv() { received = true; };
	virtual void send() { sended = true; };
};

BOOST_AUTO_TEST_CASE(ShouldNotCallbackIfNoEvent)
{
	// Given
	auto stdout = 1;	// Newer readable
	auto socket = make_shared<SocketFlagged>(stdout);
	SelectorEpoll selector(1);
	selector.addSocket(socket);
	// When
	selector.proceed();
	// Then
	BOOST_REQUIRE(!socket->received);
}

BOOST_AUTO_TEST_CASE(ShouldPollSockets)
{
	// Given
	piper p;
	auto in = make_shared<SocketFlagged>(p.in);
	auto out = make_shared<SocketFlagged>(p.out);

	SelectorEpoll selector(2);
	selector.addSocket(in);
	selector.addSocket(out);

	BOOST_REQUIRE_EQUAL(write(p.out, "X", 1), 1);
	// When
	selector.proceed();
	// Then
	BOOST_REQUIRE(in->received);
	BOOST_REQUIRE(!in->sended);
	BOOST_REQUIRE(!out->received);
	BOOST_REQUIRE(out->sended);
}

BOOST_AUTO_TEST_CASE(testShouldUseAddedSocketInProceed)
{
	// Given
	piper pfd;
	BOOST_REQUIRE_EQUAL(write(pfd.out, "X", 1), 1);

	struct SocketForRead : public SocketTest {
		bool received;
		SocketForRead(int fd) : SocketTest(fd), received(false) {};
		virtual void recv() { received = true; };
	};
	auto socket = make_shared<SocketForRead>(pfd.in);

	SelectorEpoll selector;
	selector.addSocket(socket);
	// When
	selector.proceed();
	// Then
	BOOST_REQUIRE(socket->received);
}

BOOST_AUTO_TEST_CASE(testShouldUseOutputSocketInProceed)
{
	// Given
	struct SocketForWrite : public SocketTest {
		bool sended;
		SocketForWrite(int fd) : SocketTest(fd), sended(false) {};
		virtual void send() { sended = true; };
	};
	auto socket = make_shared<SocketForWrite>(1);

	SelectorEpoll selector;
	selector.addSocket(socket);
	// When
	selector.proceed();
	// Then
	BOOST_REQUIRE(socket->sended);
}

BOOST_AUTO_TEST_CASE(testShouldNotUseRemovedSocketInProceed)
{
	// Given
	struct SocketForWrite : public SocketTest {
		bool sended;
		SocketForWrite(int fd) : SocketTest(fd), sended(false) {};
		virtual void send() { sended = true; };
	};
	auto socket = make_shared<SocketForWrite>(1);

	SelectorEpoll selector;
	selector.addSocket(socket);
	selector.addSocket(make_shared<SocketForWrite>(2));
	selector.removeSocket(socket);
	// When
	selector.proceed();
	// Then
	BOOST_REQUIRE(!socket->sended);
}

BOOST_AUTO_TEST_SUITE_END();
