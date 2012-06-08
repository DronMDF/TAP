
#include <core/ClientControl.h>

#include <boost/test/unit_test.hpp>
#include <core/TracerStream.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteClientControl);

BOOST_AUTO_TEST_CASE(ShouldPassMessageToTracer)
{
	// Given
	struct testTracer : public Tracer {
		mutable string out;
		testTracer() : out() {};
		void trace(const std::string &message) const { out = message; };
	} tracer;
	ClientControl cc(0, 0, &tracer);
	const auto message = "hello tracer";
	// When
	cc.trace(message);
	// Then
	BOOST_REQUIRE_EQUAL(tracer.out, message);
}


// TODO: Old state keep in Client, need pass it
// BOOST_AUTO_TEST_CASE(ShouldChangeStateOfClient)
// {
// 	TapManager tapm(2, [](int n){ return shared_ptr<Selector>(); },
// 		[](){ return shared_ptr<Client>(); });
// 	ClientControl cc(&tapm, 1);
// 	
// 	cc->setState(Client::CONNECTING);
// 	BOOST_REQUIRE_EQUAL(tamp.getStatistic().connecting, 1);
// 
// }
	
BOOST_AUTO_TEST_SUITE_END();
