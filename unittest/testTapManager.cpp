
#include <memory>
#include <boost/test/unit_test.hpp>
#include "../TapManager.h"
#include "../ClientBuilder.h"

using namespace std;

class Client;

BOOST_AUTO_TEST_SUITE(suiteTapManager);

struct EmptyClientBuilder : public ClientBuilder {
	virtual unsigned createSocket() const { return 0; }
	virtual shared_ptr<Client> createClient() const { return shared_ptr<Client>(); }
};

BOOST_AUTO_TEST_CASE(ShouldCallBuilderNth)
{
	// Given
	struct TestClientBuilder : public ClientBuilder {
		mutable unsigned socket_count;
		mutable unsigned client_count;
		TestClientBuilder() : socket_count(0), client_count(0) {}
		virtual unsigned createSocket() const {
			++socket_count;
			return socket_count;
		}
		virtual shared_ptr<Client> createClient() const {
			++client_count;
			return shared_ptr<Client>();
		}
	} builder;
	const unsigned nth = 1000;
	// When
	TapManager tam(nth, builder);
	// Then
	BOOST_REQUIRE_EQUAL(builder.socket_count, nth);
	BOOST_REQUIRE_EQUAL(builder.client_count, nth);
}

BOOST_AUTO_TEST_CASE(ShouldCallRecvAfterSignal)
{
	// Given
	struct TestTapManager : public TapManager {
		TestTapManager() : TapManager(3, EmptyClientBuilder()) {}
		virtual list<unsigned> selectIn() { return { 1 }; }
		virtual void receive(unsigned i) { throw i; }
	} tam;
	// Then
	BOOST_REQUIRE_EXCEPTION(tam.pressure(), unsigned, [](unsigned i){ return i == 1; });
}

BOOST_AUTO_TEST_SUITE_END();
