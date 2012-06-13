
#include <core/ClientControl.h>

#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include <core/TapManager.h>
#include <core/TracerStream.h>

using namespace std;
class Selector;

BOOST_AUTO_TEST_SUITE(suiteClientControl);

BOOST_AUTO_TEST_CASE(ShouldPassMessageToTracer)
{
	// Given
	struct testTracer : public Tracer {
		mutable string out;
		testTracer() : out() {};
		virtual void trace(const std::string &message) const { out = message; };
	} tracer;
	ClientControl cc(0, 0, &tracer);
	const auto message = "hello tracer";
	// When
	cc.trace(message);
	// Then
	BOOST_REQUIRE_EQUAL(tracer.out, message);
}

BOOST_AUTO_TEST_CASE(ShouldPassKeyValueToTracer)
{
	// Given
	struct testTracer : public Tracer {
		mutable string key;
		mutable unsigned value;
		testTracer() : key(), value(0) {};
		virtual void trace(const std::string &key, unsigned value) const 
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

BOOST_AUTO_TEST_CASE(ShouldChangeStateOfClient)
{
	struct testTapManager : public TapManager {
		unsigned n;
		int state;
		testTapManager() 
			: TapManager(0, [](int){ return shared_ptr<Selector>(); }, 
				[](){ return shared_ptr<Client>(); }), 
			  n(0), state(-1) 
		{};
		virtual void setState(unsigned n, int state) {
			this->n = n;
			this->state = state;
		}
	} tapm;
	ClientControl cc(&tapm, 42, 0);
	
	cc.setState(8);
	BOOST_REQUIRE_EQUAL(tapm.n, 42);
	BOOST_REQUIRE_EQUAL(tapm.state, 8);
}
	
BOOST_AUTO_TEST_SUITE_END();
