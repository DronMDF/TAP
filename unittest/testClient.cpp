
#include <array>
#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include <core/ClientControl.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteClient);

struct piper {
	int in;
	int out;
	piper() : in(-1), out(-1) {
		pipe(reinterpret_cast<int *>(this));
	}
	~piper() {
		close(in);
		close(out);
	}
};

BOOST_AUTO_TEST_CASE(ShouldWriteDataToMain)
{
	// Given
	struct TestClient : public Client {
		int fd;
		TestClient() : fd(-1) {};
		virtual int getMain() const { return fd; };
		virtual void readFromMain(ClientControl *) {};
		virtual void timeout(ClientControl *) {};
	} client;
	ClientControl cc(0, 0, 0);
	vector<uint8_t> expected = {'x', 'x', 'x'};
	piper p;
	// When
	client.fd = p.out;
	BOOST_REQUIRE(client.writeToMain(&cc, expected));
	// Then
	array<uint8_t, 3> buf;
	BOOST_REQUIRE_EQUAL(read(p.in, &buf, buf.size()), buf.size());
	BOOST_REQUIRE_EQUAL_COLLECTIONS(buf.begin(), buf.end(), expected.begin(), expected.end());
}
	
BOOST_AUTO_TEST_SUITE_END();

