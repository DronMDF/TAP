
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(suiteClientControl);

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
