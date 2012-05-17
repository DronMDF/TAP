
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
