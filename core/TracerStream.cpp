
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

void TracerStream::trace(const std::string &key, unsigned value) const
{
	(*out) << timestamp() << key << ": " << value << endl;
}

