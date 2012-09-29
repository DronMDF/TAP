
#include <iostream>
#include "TracerStream.h"

using namespace std;

TracerStream::TracerStream(ostream *out, std::function<std::string()> timestamp)
	: out(out), timestamp(timestamp)
{
}

void TracerStream::trace(unsigned id, const string &msg) const
{
	const string ts = timestamp();
	(*out) << ts << (ts.empty() ? "" : " ") << id << ' ' << msg << endl;
}

void TracerStream::trace(unsigned id, const std::string &key, unsigned value) const
{
	const string ts = timestamp();
	(*out) << ts << (ts.empty() ? "" : " ") << id << ' ' << key << ": " << value << endl;
}

