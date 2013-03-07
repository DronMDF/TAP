
#include <core/ClientControl.h>

#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include <core/TapManager.h>
#include <core/TracerStream.h>
#include "TapTestCase.h"

using namespace std;
class Selector;

BOOST_AUTO_TEST_SUITE(suiteClientControl);

TAP_TEST_CASE(ShouldPassMessageToTracer)
{
	// Given
	struct testTracer : public Tracer {
		mutable string out;
		testTracer() : out() {};
		virtual void trace(unsigned, const std::string &message) const { out = message; };
	} tracer;
	ClientControl cc(0, 0, &tracer);
	const auto message = "hello tracer";
	// When
	cc.trace(message);
	// Then
	BOOST_REQUIRE_EQUAL(tracer.out, message);
}

TAP_TEST_CASE(ShouldPassKeyValueToTracer)
{
	// Given
	struct testTracer : public Tracer {
		mutable string key;
		mutable unsigned value;
		testTracer() : key(), value(0) {};
		virtual void trace(unsigned, const std::string &key, unsigned value) const
			{ this->key = key; this->value = value; };
	} tracer;
	ClientControl cc(0, 0, &tracer);
	const auto key = "keyname";
	const auto value = 42U;
	// When
	cc.trace(key, value);
	// Then
	BOOST_REQUIRE_EQUAL(tracer.key, key);
	BOOST_REQUIRE_EQUAL(tracer.value, value);
}

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
		virtual void setStateOffline(unsigned n) { this->n = n; }
	} tapm;
	ClientControl cc(&tapm, 42, 0);
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
		virtual void setStateConnecting(unsigned n) { this->n = n; }
	} tapm;
	ClientControl cc(&tapm, 42, 0);
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
		virtual void setStateOnline(unsigned n) { this->n = n; }
	} tapm;
	ClientControl cc(&tapm, 42, 0);
	// When
	cc.setStateOnline();
	// Then
	BOOST_REQUIRE_EQUAL(tapm.n, 42);
}
	
BOOST_AUTO_TEST_SUITE_END();
