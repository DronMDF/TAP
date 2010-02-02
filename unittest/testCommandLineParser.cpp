
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "../CommandLineParser.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(suiteCommandLineParser);

BOOST_AUTO_TEST_CASE(testHelp)
{
	const char *argv[] = { "xap", "help", };
	CommandLineParser cmd(2, argv);

	ostringstream out;
	cmd.run(out);

	BOOST_REQUIRE_EQUAL(out.str(), CommandLineParser::help);
}

BOOST_AUTO_TEST_SUITE_END();
