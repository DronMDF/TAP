
#include <iostream>
#include "CommandLineParser.h"

using namespace std;

int main(int argc, const char **argv)
{
	CommandLineParser cmd(argc - 1, argv + 1);
	return cmd.run(cout);
}
