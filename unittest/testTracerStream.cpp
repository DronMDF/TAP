
#include <boost/test/unit_test.hpp>
#include <core/TracerStream.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteTracerStream);

// TODO: ����������� ������ �������������� ���������� �������, �� ���� ��������� �� ������������
BOOST_AUTO_TEST_CASE(ShouldOutString)
{
	// Given
	ostringstream out;
	TracerStream tracer(&out);
	// When
	tracer.trace("hello");
	// Then
	BOOST_REQUIRE_EQUAL(out.str(), "hello\n");
}

BOOST_AUTO_TEST_SUITE_END();
