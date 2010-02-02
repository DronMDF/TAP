
#include <sstream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include <boost/test/utils/nullstream.hpp>

#include "../CommandLineParser.h"
#include "testUtility.h"

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE(suiteCommandLineParser);

BOOST_AUTO_TEST_CASE(testHelp)
{
	const char *argv[] = { "xap", "help", };
	CommandLineParser cmd(2, argv);

	ostringstream out;
	cmd.run(out);

	BOOST_REQUIRE_EQUAL(out.str(), CommandLineParser::help);
}

BOOST_AUTO_TEST_CASE(testServer)
{
	const char *argv[] = { "xap", "-s", "test:1234", "load", };
	
	struct testCLP : public CommandLineParser {
		testCLP(int argc, const char **argv) : CommandLineParser(argc, argv) {};
		int runLoad(const string &server, const string &, const string &, int) {
			throw runtime_error(server);
		}
		
	} cmd(4, argv);

	onullstream out;
	BOOST_REQUIRE_EXCEPTION(cmd.run(out), runtime_error,
		check_exception_message(argv[2]));
}


BOOST_AUTO_TEST_SUITE_END();
