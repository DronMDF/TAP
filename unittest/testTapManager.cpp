
#include <memory>
#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include <core/ClientBuilder.h>
#include <core/TapManager.h>
#include "SelectorTest.h"

using namespace std;

class Client;

BOOST_AUTO_TEST_SUITE(suiteTapManager);

template <typename C>
struct TestClientBuilder : public ClientBuilder {
	virtual int createSocket() const { return 66; }
	virtual shared_ptr<Client> createClient() const { return make_shared<C>(); }
};

BOOST_AUTO_TEST_CASE(ShouldCallBuilderNth)
{
	// Given
	struct TestClientBuilder : public ClientBuilder {
		mutable int socket_count;
		mutable int client_count;
		TestClientBuilder() : socket_count(0), client_count(0) {}
		virtual int createSocket() const {
			++socket_count;
			return socket_count;
		}
		virtual shared_ptr<Client> createClient() const {
			++client_count;
			return shared_ptr<Client>();
		}
	} builder;
	const int nth = 1000;
	// When
	TapManager tam(nth, [](int){ return make_shared<SelectorTest>(); }, builder);
	// Then
	BOOST_REQUIRE_EQUAL(builder.socket_count, nth);
	BOOST_REQUIRE_EQUAL(builder.client_count, nth);
}

BOOST_AUTO_TEST_CASE(ShouldWakeupClientAtFirst)
{
	// Given
	struct wakeuped {};
	struct TestClient : public Client {
		virtual void wakeup() { throw wakeuped(); };
	};
	struct TestTapManager : public TapManager {
		TestTapManager() 
			: TapManager(1, [](int){ return make_shared<SelectorTest>(); }, 
				TestClientBuilder<TestClient>()) 
		{ }
	} tam;
	// Then
	BOOST_REQUIRE_THROW(tam.pressure(), wakeuped);
}

BOOST_AUTO_TEST_SUITE_END();
