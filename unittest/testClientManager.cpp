
#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>

#include "../Client.h"
#include "../ClientManager.h"

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE(suiteClientManager);

static int push_count = 10;
struct push_exception {};

struct testClient : public Client {
	void push() {
		if (--push_count <= 0) throw push_exception();
	}
};
Client *createTestClient() { return new testClient; }

struct testClientManager : public ClientManager {
	testClientManager() : ClientManager(createTestClient, 10) {}
	using ClientManager::m_clients;
};

BOOST_AUTO_TEST_CASE(testClientManagerInit)
{
	testClientManager mgr;
	BOOST_REQUIRE_EQUAL(mgr.m_clients.size(), ClientManager::clients_list_t::size_type(10));
	BOOST_REQUIRE(typeid(*(mgr.m_clients.front())) == typeid(testClient));
}

BOOST_AUTO_TEST_CASE(testClientManagerRun)
{
	testClientManager mgr;
	push_count = mgr.m_clients.size() * 2;
	BOOST_REQUIRE_THROW(mgr.run(), push_exception);
}

BOOST_AUTO_TEST_SUITE_END();
