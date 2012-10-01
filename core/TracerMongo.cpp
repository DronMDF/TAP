
#include "TracerMongo.h"

#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace mongo;

TracerMongo::TracerMongo(const string &server)
	: client()
{
	client.connect(server);
}

TracerMongo::~TracerMongo()
{
	dbexit(EXIT_CLEAN);
}

void TracerMongo::trace(unsigned id, const string &msg) const
{
	BSONObjBuilder bson;
	const auto time = high_resolution_clock::now();
	bson.append("timestamp", duration_cast<milliseconds>(time.time_since_epoch()).count());
	bson.append("cid", id);
	bson.append("message", msg);
	client.insert("test.tap", bson.obj());
}

void TracerMongo::trace(unsigned id, const std::string &key, unsigned value) const
{
	BSONObjBuilder bson;
	const auto time = high_resolution_clock::now();
	bson.append("timestamp", duration_cast<milliseconds>(time.time_since_epoch()).count());
	bson.append("cid", id);
	bson.append("name", key);
	bson.append("value", value);
	client.insert("test.tap", bson.obj());
}
