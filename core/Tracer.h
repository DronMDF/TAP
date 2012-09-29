
#pragma once
#include <string>

// Tracer interface and empty tracer
class Tracer {
public:
	virtual ~Tracer();
	virtual void trace(unsigned id, const std::string &msg) const;
	virtual void trace(unsigned id, const std::string &key, unsigned value) const;
};

