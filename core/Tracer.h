
#pragma once
#include <string>

// Это одновременно интерфейс и пустой трейсер
class Tracer {
public:
	virtual ~Tracer();
	virtual void trace(const std::string &msg) const;
	virtual void trace_throughput(unsigned bytes_per_sec) const;
};
