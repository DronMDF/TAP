
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

BOOST_AUTO_TEST_CASE(ShouldNotCallbackUninitialized)
{
	// Given
	SelectorPoll selector(10);
	selector.select();
	// When/Then
	selector.selectRead([](int){ BOOST_FAIL("Invalid Call"); });
}

BOOST_AUTO_TEST_CASE(ShouldNotCallbackOnWriteIfUninitialized)
{
	// Given
	SelectorPoll selector(10);
	selector.select();
	// When/Then
	selector.selectWrite([](int){ BOOST_FAIL("Invalid call"); });
}

BOOST_AUTO_TEST_CASE(ShouldNotCallbackIfNoEvent)
{
	// Given
	SelectorPoll selector(10);
	for (int i = 0; i < 10; i++) {
		selector.setSocket(i, make_shared<SocketTest>(1));
	}
	selector.select();
	// When/Then
	selector.selectRead([](int){ BOOST_FAIL("Invalid Call"); });
}

BOOST_AUTO_TEST_CASE(ShouldReturnIndexOfReadableDescriptorByCallback)
{
	// Given
	SelectorPoll selector(10);
	for (int i = 0; i < 10; i++) {
		selector.setSocket(i, make_shared<SocketTest>(1));
	}
	piper p;
	selector.setSocket(5, make_shared<SocketTest>(p.in));
	BOOST_REQUIRE_EQUAL(write(p.out, "X", 1), 1);
	selector.select();
	// When
	int rv;
	selector.selectRead([&rv](int n){ rv = n; });
	// Then
	BOOST_REQUIRE_EQUAL(rv, 5);
}

BOOST_AUTO_TEST_CASE(ShouldNotCallIfNoOneReadyForWrite)
{
	// Given
	SelectorPoll selector(10);
	piper p;
	for (int i = 0; i < 10; i++) {
		selector.setDescriptor(i, p.in);
	}
	selector.select();
	// When/Then
	selector.selectWrite([](int){ BOOST_FAIL("Invalid call"); });
}

BOOST_AUTO_TEST_CASE(ShouldCallIndexOfWritableDescriptor)
{
	// Given
	SelectorPoll selector(10);
	piper p;
	for (int i = 0; i < 10; i++) {
		selector.setSocket(i, make_shared<SocketTest>(p.in));
	}
	selector.setSocket(5, make_shared<SocketTest>(p.out));
	selector.select();
	// When
	int rv = -1;
	selector.selectWrite([&rv](int n){
		BOOST_REQUIRE_EQUAL(n, 5);
		rv = n;
	});
	// Then
	BOOST_REQUIRE_EQUAL(rv, 5);
}

BOOST_AUTO_TEST_SUITE_END();
