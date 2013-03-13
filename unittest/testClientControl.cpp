
#include <core/ClientControl.h>

#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include <core/TapManager.h>
#include <core/TracerStream.h>
#include "TapTestCase.h"

using namespace std;
class Selector;

BOOST_AUTO_TEST_SUITE(suiteClientControl);

struct testStateTapManager : public TapManager {
	testStateTapManager() : TapManager(0, [](int){ return shared_ptr<Selector>(); }, 
			[](){ return shared_ptr<Client>(); }) {};
};

TAP_TEST_CASE(ShouldChangeStateOfClientToOffline)
{
	// Given
	struct testTapManager : public testStateTapManager {
		unsigned n;
		testTapManager() : n(0) {};
		virtual void setStateOffline(unsigned n) override { this->n = n; }
	} tapm;
	ClientControl cc(&tapm, 42);
	// When
	cc.setStateOffline();
	// Then
	BOOST_REQUIRE_EQUAL(tapm.n, 42);
}
	
TAP_TEST_CASE(ShouldChangeStateOfClientToConnected)
{
	// Given
	struct testTapManager : public testStateTapManager {
		unsigned n;
		testTapManager() : n(0) {};
		virtual void setStateConnecting(unsigned n) override { this->n = n; }
	} tapm;
	ClientControl cc(&tapm, 42);
	// When
	cc.setStateConnecting();
	// Then
	BOOST_REQUIRE_EQUAL(tapm.n, 42);
}
	
TAP_TEST_CASE(ShouldChangeStateOfClientToOnline)
{
	// Given
	struct testTapManager : public testStateTapManager {
		unsigned n;
		testTapManager() : n(0) {};
		virtual void setStateOnline(unsigned n) override { this->n = n; }
	} tapm;
	ClientControl cc(&tapm, 42);
	// When
	cc.setStateOnline();
	// Then
	BOOST_REQUIRE_EQUAL(tapm.n, 42);
}
	
BOOST_AUTO_TEST_SUITE_END();
