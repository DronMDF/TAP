
#include <memory.h>
#include <boost/test/unit_test.hpp>
#include <core/SelectorPoll.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteSelectorPoll);

struct piper {
	int in;
	int out;
	piper() : in(-1), out(-1) {
		pipe(reinterpret_cast<int *>(this));
	}
	~piper() {
		close(in);
		close(out);
	}
};

BOOST_AUTO_TEST_CASE(ShouldReturnFirstUninitialized)
{
	// Given
	SelectorPoll selector(10);
	// Then
	BOOST_REQUIRE_EQUAL(selector.selectRead(), 0);
}

BOOST_AUTO_TEST_CASE(ShouldCreateEmptySet)
{
	// Given
	SelectorPoll selector(10);
	// Then
	BOOST_REQUIRE_EQUAL(selector.select(), -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnIndexOfReadableDescriptor)
{
	// Given
	piper p;
	SelectorPoll selector(10);
	selector.setDescriptor(5, p.in);
	write(p.out, "X", 1);
	// Then
	BOOST_REQUIRE_EQUAL(selector.select(), 5);
}


BOOST_AUTO_TEST_SUITE_END();
