
#pragma once
#include <memory>
#include "Tracer.h"

namespace mongo { class DBClientConnection; };

class TracerMongo : public Tracer {
public:
	TracerMongo(const std::string &server);
	~TracerMongo();
	
	virtual void trace(unsigned id, const std::string &msg) const;
	virtual void trace(unsigned id, const std::string &key, unsigned value) const;

private:
	TracerMongo(const TracerMongo&) = delete;
	TracerMongo &operator=(const TracerMongo&) = delete;
	
	std::unique_ptr<mongo::DBClientConnection> client;
};
