
#pragma once

#include <iosfwd>
#include <functional>
#include "Tracer.h"

class TracerStream : public Tracer {
public:
	TracerStream(std::ostream *out, 
		std::function<std::string()> timestamp = [](){ return ""; });
	
	virtual void trace(const std::string &msg) const;
	virtual void trace_throughput(unsigned bytes_per_sec) const;

private:
	TracerStream(const TracerStream&) = delete;
	TracerStream &operator=(const TracerStream&) = delete;
	
	std::ostream *out;
	std::function<std::string()> timestamp;
};
