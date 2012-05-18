
#include <iostream>
#include "TracerStream.h"

using namespace std;

TracerStream::TracerStream(ostream *out)
	: out(out)
{
}

void TracerStream::trace(const string &msg) const
{
	(*out) << msg << endl;
}

void TracerStream::trace_throughput(unsigned bytes_per_sec) const
{
	(*out) << "Throughput: " << bytes_per_sec << "bps" << endl;
}
