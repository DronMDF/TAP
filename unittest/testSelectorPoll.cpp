
#include <boost/test/unit_test.hpp>
#include "../SelectorPoll.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteSelectorPoll);

BOOST_AUTO_TEST_CASE(ShouldCreateEmptySet)
{
	// Given
	shared_ptr<Selector> selector = make_shared<SelectorPoll>(10);
	// Then
	BOOST_REQUIRE_EQUAL(selector->select(), -1);
}

BOOST_AUTO_TEST_CASE(ShouldReturnIndexOfReadableDescriptor)
{
	// Given
	shared_ptr<Selector> selector = make_shared<SelectorPoll>(10);
	int fd[2];
	pipe(fd);
	selector->setDescriptor(5, fd[0]);
	write(fd[1], &fd, sizeof(fd));
	close(fd[1]);
	// Then
	BOOST_REQUIRE_EQUAL(selector->select(), 5);
	close(fd[0]);
}


BOOST_AUTO_TEST_SUITE_END();
