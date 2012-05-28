
#include <memory>
#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include <core/TapManager.h>
#include "SelectorTest.h"

using namespace std;

class Client;

BOOST_AUTO_TEST_SUITE(suiteTapManager);

struct ClientStub : public Client {
	virtual int getMain() const { return -1; };
	virtual void readFromMain(ClientControl *) {};
	virtual void timeout(ClientControl *) {};
};

BOOST_AUTO_TEST_CASE(ShouldCallBuilderNth)
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

BOOST_AUTO_TEST_CASE(ShouldCallActivate)
{
	// Given
	struct was_action {};
	struct TestClient : public ClientStub {
		virtual void action(ClientControl *) { throw was_action(); };
	};
	// When
	TapManager tam(1, [](int){ return make_shared<SelectorTest>(); }, 
		[](){ return make_shared<TestClient>(); });
	// Then
	BOOST_REQUIRE_THROW(tam.pressure(), was_action);
}

BOOST_AUTO_TEST_SUITE_END();
