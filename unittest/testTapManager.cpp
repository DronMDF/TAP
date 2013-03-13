
#include <core/TapManager.h>

#include <memory>
#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include "SelectorTest.h"
#include "TapTestCase.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteTapManager);

struct ClientStub : public Client {
	virtual void timeout(ClientControl *) override {};
};

TAP_TEST_CASE(ShouldCallBuilderNth)
{
	// Given
	int actual = 0;
	auto create_selector = [](int){ return make_shared<SelectorTest>(); };
	auto create_client = [&actual](){ actual++; return make_shared<ClientStub>(); };

	const int nth = 1000;
	// When
	TapManager tam(nth, create_selector, create_client);
	// Then
	BOOST_REQUIRE_EQUAL(actual, nth);
}

TAP_TEST_CASE(ShouldCallActivate)
{
	// Given
	struct was_action {};
	struct TestClient : public ClientStub {
		virtual void action(ClientControl *) override { throw was_action(); };
	};
	// When
	TapManager tam(1, [](int){ return make_shared<SelectorTest>(); }, 
		[](){ return make_shared<TestClient>(); });
	// Then
	BOOST_REQUIRE_THROW(tam.pressure(), was_action);
}

TAP_TEST_CASE(ShouldInitAllClientAsOffline)
{
	// Given
	TapManager tam(10, 
		[](int){ return shared_ptr<Selector>(); }, 
		[](){ return make_shared<ClientStub>(); });
	// When
	tam.setShowStatistic([](int offline, int, int){ throw offline; }, chrono::seconds::min());
	// Then
	BOOST_REQUIRE_EXCEPTION(tam.pressure(), int, [](int n){ return n == 10; });
}

BOOST_AUTO_TEST_SUITE_END();
