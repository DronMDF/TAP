
#include <memory.h>
#include <boost/test/unit_test.hpp>
#include <core/SelectorPoll.h>
#include <core/Socket.h>

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

struct TestSocket : public Socket {
	int fd;
	TestSocket(int fd) : fd(fd) {};
	virtual int getDescriptor() const { return fd; };
	virtual vector<uint8_t> recv() { return {}; };
	virtual size_t send(const vector<uint8_t> &) { return 0; };
};

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeUninitialized)
{
	// Given
	SelectorPoll selector(10);
	// When
	int rv = selector.selectRead();
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeIfNoIntrest)
{
	// Given
	SelectorPoll selector(10);
	// When
	int rv = selector.selectWrite({});
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeIfNoEvent)
{
	// Given
	SelectorPoll selector(10);
	for (int i = 0; i < 10; i++) {
		selector.setDescriptor(i, 1);
	}
	// When
	int rv = selector.selectRead();
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeIfNoEventBySocket)
{
	// Given
	SelectorPoll selector(10);
	for (int i = 0; i < 10; i++) {
		selector.setSocket(i, make_shared<TestSocket>(1));
	}
	// When
	int rv = selector.selectRead();
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnIndexOfReadableDescriptor)
{
	// Given
	SelectorPoll selector(10);
	for (int i = 0; i < 10; i++) {
		selector.setDescriptor(i, 1);
	}
	piper p;
	selector.setDescriptor(5, p.in);
	write(p.out, "X", 1);
	// After poll return -1 always
	BOOST_REQUIRE_EQUAL(selector.selectRead(), -1);
	// When
	int rv = selector.selectRead();
	// Then
	BOOST_REQUIRE_EQUAL(rv, 5);
}

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeIfNoWrite)
{
	// Given
	SelectorPoll selector(10);
	piper p;
	for (int i = 0; i < 10; i++) {
		selector.setDescriptor(i, p.in);
	}
	// When
	int rv = selector.selectWrite({3, 5, 7});
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnIndexOfWritableDescriptor)
{
	// Given
	SelectorPoll selector(10);
	piper p;
	for (int i = 0; i < 10; i++) {
		selector.setDescriptor(i, (i == 5) ? p.out : p.in);
	}
	// When
	int rv = selector.selectWrite({3, 5, 7});
	// Then
	BOOST_REQUIRE_EQUAL(rv, 5);
}

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeAfterLast)
{
	// Given
	SelectorPoll selector(10);
	piper p;
	for (int i = 0; i < 10; i++) {
		selector.setDescriptor(i, (i == 9) ? p.out : p.in);
	}
	BOOST_REQUIRE_EQUAL(selector.selectWrite({3, 5, 7, 9}), 9);
	// When
	int rv = selector.selectWrite({});
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_SUITE_END();
