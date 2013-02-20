
#include <boost/test/unit_test.hpp>
#include <core/Selector.h>
#include "SocketTest.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteSelector);

struct testedSelector : public Selector {
	virtual void proceed() { }
	using Selector::sockets;
};

BOOST_AUTO_TEST_CASE(testShouldAddSocket)
{
	// Given
	testedSelector selector;
	auto socket = make_shared<SocketTest>(123);
	// When
	selector.addSocket(socket);
	// Then
	BOOST_REQUIRE_EQUAL(selector.sockets.at(socket->getDescriptor()), socket);
}

BOOST_AUTO_TEST_CASE(testShouldRemoveSocket)
{
	// Given
	testedSelector selector;
	auto socket = make_shared<SocketTest>(321);
	selector.addSocket(socket);
	// When
	selector.removeSocket(socket);
	// Then
	BOOST_REQUIRE_THROW(selector.sockets.at(socket->getDescriptor()), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()