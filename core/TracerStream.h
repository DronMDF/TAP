
#pragma once
#include "Tracer.h"
#include <iosfwd>

class TracerStream : public Tracer {
public:
	TracerStream(std::ostream *out);
	
	virtual void trace(const std::string &msg) const;
	virtual void trace_throughput(unsigned bytes_per_sec) const;

private:
	std::ostream *out;
	
	TracerStream(const TracerStream&) = delete;
	TracerStream &operator=(const TracerStream&) = delete;
};
