
#pragma once
#include <string>

// Tracer interface and empty tracer
class Tracer {
public:
	virtual ~Tracer();
	virtual void trace(const std::string &msg) const;
	virtual void trace(const std::string &key, unsigned value) const;
	
	virtual void trace_throughput(unsigned bytes_per_sec) const __attribute__((deprecated));
};

