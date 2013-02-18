
#include "TracerMongo.h"

#include <chrono>
#include <mongo/client/dbclient.h>

using namespace std;
using namespace std::chrono;
using namespace mongo;

static
void mongo_exit()
{
	dbexit(EXIT_CLEAN);
}

TracerMongo::TracerMongo(const string &server)
	: client(new DBClientConnection())
{
	client->connect(server);
	atexit(mongo_exit);
}

TracerMongo::~TracerMongo()
{
}

void TracerMongo::trace(unsigned id, const string &msg) const
{
	BSONObjBuilder bson;
	const auto time = high_resolution_clock::now();
	const long long timestamp = duration_cast<milliseconds>(time.time_since_epoch()).count();
	bson.append("timestamp", timestamp);
	bson.append("cid", id);
	bson.append("message", msg);
	client->insert("test.tap", bson.obj());
}

void TracerMongo::trace(unsigned id, const std::string &key, unsigned value) const
{
	BSONObjBuilder bson;
	const auto time = high_resolution_clock::now();
	const long long timestamp = duration_cast<milliseconds>(time.time_since_epoch()).count();
	bson.append("timestamp", timestamp);
	bson.append("cid", id);
	bson.append("name", key);
	bson.append("value", value);
	client->insert("test.tap", bson.obj());
}
