
#include <iostream>
#include "TracerStream.h"

using namespace std;

TracerStream::TracerStream(ostream *out, std::function<std::string()> timestamp)
	: out(out), timestamp(timestamp)
{
}

void TracerStream::trace(const string &msg) const
{
	(*out) << timestamp() << msg << endl;
}

void TracerStream::trace_throughput(unsigned bytes_per_sec) const
{
	(*out) << timestamp() << "Throughput: " << bytes_per_sec << "bps" << endl;
}
