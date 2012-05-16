
#include <memory>
#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include <core/TapManager.h>
#include "SelectorTest.h"

using namespace std;

class Client;

BOOST_AUTO_TEST_SUITE(suiteTapManager);

struct ClientStub : public Client {
	virtual int createMainDescriptor() { return -1; };
	virtual void wakeup() {};
};

BOOST_AUTO_TEST_CASE(ShouldCallBuilderNth)
{
	// Given
	int actual = 0;
	auto create_selector = [](int){ return make_shared<SelectorTest>(); };
	auto create_client = [&actual](){ actual++; return shared_ptr<Client>(); };

	const int nth = 1000;
	// When
	TapManager tam(nth, create_selector, create_client);
	// Then
	BOOST_REQUIRE_EQUAL(actual, nth);
}

// BOOST_AUTO_TEST_CASE(ShouldWakeupClientAtFirst)
// {
// 	// Given
// 	struct wakeuped {};
// 	struct TestClient : public ClientStub {
// 		virtual void wakeup() { throw wakeuped(); };
// 	};
// 	struct TestTapManager : public TapManager {
// 		TestTapManager() 
// 			: TapManager(1, [](int){ return make_shared<SelectorTest>(); }, 
// 				TestClientBuilder<TestClient>()) 
// 		{ }
// 	} tam;
// 	// Then
// 	BOOST_REQUIRE_THROW(tam.pressure(), wakeuped);
// }

BOOST_AUTO_TEST_SUITE_END();
