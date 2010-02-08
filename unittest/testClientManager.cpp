
#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>

#include "../Client.h"
#include "../ClientManager.h"

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE(suiteClientManager);

struct testClient : public Client {};
Client *createTestClient() { return new testClient; }

BOOST_AUTO_TEST_CASE(testClientManagerInit)
{
	struct testClientManager : public ClientManager {
		testClientManager() : ClientManager(createTestClient, 10) {}
		using ClientManager::m_clients;
	} mgr;

	BOOST_REQUIRE_EQUAL(mgr.m_clients.size(), ClientManager::clients_list_t::size_type(10));
	BOOST_REQUIRE(typeid(*(mgr.m_clients.front())) == typeid(testClient));
}


BOOST_AUTO_TEST_SUITE_END();
