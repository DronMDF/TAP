

#include <array>
#include <boost/test/unit_test.hpp>
#include <core/Client.h>
#include <core/ClientControl.h>
#include <core/Socket.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteClient);

struct piper {
	int in;
	int out;
	piper() : in(-1), out(-1) {
		assert(pipe(reinterpret_cast<int *>(this)) == 0);
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
		virtual void read(ClientControl *) {};
		virtual void timeout(ClientControl *) {};
	} client;
	ClientControl cc(0, 0, 0);
	vector<uint8_t> expected = {'x', 'x', 'x'};
	piper p;
	// When
	client.fd = p.out;
	BOOST_REQUIRE(client.write(&cc, expected));
	// Then
	array<uint8_t, 3> buf;
	BOOST_REQUIRE_EQUAL(read(p.in, &buf, buf.size()), buf.size());
	BOOST_REQUIRE_EQUAL_COLLECTIONS(buf.begin(), buf.end(), expected.begin(), expected.end());
}
	
BOOST_AUTO_TEST_CASE(ShouldWantsToWriteIfQueued)
{
	// Given
	struct TestClient : public Client {
		using Client::writeToQueue;
		// stubs
		virtual int getMain() const { return 0; };
		virtual void read(ClientControl *) {};
		virtual void timeout(ClientControl *) {};
	} client;
	// When
	client.writeToQueue({});
	// Then
	BOOST_REQUIRE(client.wantsToWrite());
}

BOOST_AUTO_TEST_CASE(ShouldSendFromQueueToSocket)
{
	// Given
	struct TestClient : public Client {
		using Client::writeToQueue;
		using Client::sendFromQueue;
		// stubs
		virtual int getMain() const { return 0; };
		virtual void read(ClientControl *) {};
		virtual void timeout(ClientControl *) {};
	} client;
	struct TestSocket : public Socket {
		vector<uint8_t> sended;
		TestSocket() : sended() {}
		virtual int getDescriptor() const { return 0; };
		virtual vector<uint8_t> recv(size_t) { return {}; };
		virtual size_t send(const vector<uint8_t> &data) {
			sended = data; return data.size();
		}
	} socket;
	const vector<uint8_t> packet = {1, 2, 3};
	// When
	client.writeToQueue(packet);
	client.sendFromQueue(&socket);
	// Then
	BOOST_REQUIRE(socket.sended == packet);
}

BOOST_AUTO_TEST_SUITE_END();

