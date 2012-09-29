
#pragma once

#include <iosfwd>
#include <functional>
#include "Tracer.h"

class TracerStream : public Tracer {
public:
	TracerStream(std::ostream *out, 
		std::function<std::string()> timestamp = [](){ return ""; });
	
	virtual void trace(unsigned id, const std::string &msg) const;
	virtual void trace(unsigned id, const std::string &key, unsigned value) const;

private:
	TracerStream(const TracerStream&) = delete;
	TracerStream &operator=(const TracerStream&) = delete;
	
	std::ostream *out;
	std::function<std::string()> timestamp;
};
