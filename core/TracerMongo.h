
#pragma once
#include <mongo/client/dbclient.h>
#include "Tracer.h"

class TracerMongo : public Tracer {
public:
	TracerMongo(const std::string &server);
	~TracerMongo();
	
	virtual void trace(unsigned id, const std::string &msg) const;
	virtual void trace(unsigned id, const std::string &key, unsigned value) const;

private:
	TracerMongo(const TracerMongo&) = delete;
	TracerMongo &operator=(const TracerMongo&) = delete;
	
	mutable mongo::DBClientConnection client;
};
