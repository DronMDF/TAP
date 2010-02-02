
#include <iostream>
#include "CommandLineParser.h"

using namespace std;

int main(int argc, const char **argv)
{
	CommandLineParser cmd(argc, argv);
	return cmd.run(cout);
}
