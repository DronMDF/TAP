
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

BOOST_AUTO_TEST_CASE(ShouldNotCallbackUninitialized)
{
	// Given
	SelectorEpoll selector(10);
	selector.select();
	// When/Then
	selector.selectRead([](int){ BOOST_FAIL("Invalid Call"); });
}

BOOST_AUTO_TEST_CASE(ShouldNotCallbackOnWriteIfUninitialized)
{
	// Given
	SelectorEpoll selector(10);
	selector.select();
	// When/Then
	selector.selectWrite([](int){ BOOST_FAIL("Invalid call"); });
}

BOOST_AUTO_TEST_CASE(ShouldNotCallbackIfNoEvent)
{
	// Given
	SelectorEpoll selector(1);
	selector.setSocket(0, make_shared<SocketTest>(1));
	// When
	selector.select();
	// Then
	selector.selectRead([](int){ BOOST_FAIL("Invalid Call"); });
}

BOOST_AUTO_TEST_CASE(ShouldReturnIndexOfReadableDescriptorByCallback)
{
	// Given
	piper p;
	SelectorEpoll selector(2);
	selector.setSocket(0, make_shared<SocketTest>(1));
	selector.setSocket(1, make_shared<SocketTest>(p.in));
	BOOST_REQUIRE_EQUAL(write(p.out, "X", 1), 1);
	// When
	selector.select();
	int rv;
	selector.selectRead([&rv](int n){ rv = n; });
	// Then
	BOOST_REQUIRE_EQUAL(rv, 1);
}

BOOST_AUTO_TEST_CASE(ShouldCallIndexOfWritableDescriptor)
{
	// Given
	piper p;
	SelectorEpoll selector(2);
	selector.setSocket(0, make_shared<SocketTest>(p.in));
	selector.setSocket(1, make_shared<SocketTest>(p.out));
	selector.select();
	// When
	int rv = -1;
	selector.selectWrite([&rv](int n){
		BOOST_REQUIRE_EQUAL(n, 1);
		rv = n;
	});
	// Then
	BOOST_REQUIRE_EQUAL(rv, 1);
}

BOOST_AUTO_TEST_CASE(ShouldPollSockets)
{
	// Given
	struct SocketFlagged : public SocketTest {
		bool received, sended;
		SocketFlagged(int fd) : SocketTest(fd), received(false), sended(false) {};
		virtual void recv() { received = true; };
		virtual void send() { sended = true; };
	};

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

BOOST_AUTO_TEST_SUITE_END();
