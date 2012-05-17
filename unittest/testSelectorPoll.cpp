
#include <memory.h>
#include <boost/test/unit_test.hpp>
#include <core/SelectorPoll.h>

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

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeUninitialized)
{
	// Given
	SelectorPoll selector(10);
	// When
	int rv = selector.selectRead();
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeUninitializedOnWrite)
{
	// Given
	SelectorPoll selector(10);
	// When
	int rv = selector.selectWrite();
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnNegativeIfNoEvent)
{
	// Given
	SelectorPoll selector(10);
	vector<piper> pipers(10);
	for (int i = 0; i < 10; i++) {
		selector.setDescriptor(i, 1);
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
	// Then
	BOOST_REQUIRE_EQUAL(selector.selectWrite(), -1);
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
	int rv = selector.selectWrite();
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
	BOOST_REQUIRE_EQUAL(selector.selectWrite(), 9);
	// When
	int rv = selector.selectWrite();
	// Then
	BOOST_REQUIRE_EQUAL(rv, -1);
}

BOOST_AUTO_TEST_SUITE_END();
