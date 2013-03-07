
#include <core/TracerStream.h>

#include <boost/test/unit_test.hpp>
#include "TapTestCase.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteTracerStream);

TAP_TEST_CASE(ShouldOutString)
{
	// Given
	ostringstream out;
	TracerStream tracer(&out);
	// When
	tracer.trace(42, "hello");
	// Then
	BOOST_REQUIRE_EQUAL(out.str(), "42 hello\n");
}

TAP_TEST_CASE(ShouldPrintTimestampBeforeString)
{
	// Given
	ostringstream out;
	TracerStream tracer(&out, [](){ return "[timestamp]"; });
	// When
	tracer.trace(66, "hello");
	// Then
	BOOST_REQUIRE_EQUAL(out.str(), "[timestamp] 66 hello\n");
}

BOOST_AUTO_TEST_SUITE_END();
