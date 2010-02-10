
#include <sstream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include <boost/test/utils/nullstream.hpp>
#include <boost/function.hpp>

#include "../CommandLineParser.h"
#include "../Client.h"
#include "../ClientHonest.h"
#include "../ClientManager.h"
#include "testUtility.h"

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE(suiteCommandLineParser);

static onullstream nullout;

BOOST_AUTO_TEST_CASE(testHelp)
{
	const char *argv[] = { "help", };
	CommandLineParser cmd(sizeof(argv) / sizeof(argv[0]), argv);
	ostringstream out;
	cmd.run(out);
	BOOST_REQUIRE_EQUAL(out.str(), CommandLineParser::help);
}

struct testCommandLineParser : public CommandLineParser {
	testCommandLineParser()	: CommandLineParser(0, 0) {};
	testCommandLineParser(int argc, const char **argv)
		: CommandLineParser(argc, argv)
	{};

	using CommandLineParser::m_options;
	using CommandLineParser::m_args;

	using CommandLineParser::createClientHonest;
};

BOOST_AUTO_TEST_CASE(testOptions)
{
	const string server = "test:1234";
	const string cert = "cert.test";
	const string key = "key.test";
	const string num = "8000";
	const char *argv[] = {
		"-s", server.c_str(),
		"-c", cert.c_str(),
		"-k", key.c_str(),
		"-n", num.c_str()
	};

	testCommandLineParser cmd(sizeof(argv) / sizeof(argv[0]), argv);
	
	BOOST_REQUIRE_EQUAL(cmd.m_options["server"], server);
	BOOST_REQUIRE_EQUAL(cmd.m_options["certfile"], cert);
	BOOST_REQUIRE_EQUAL(cmd.m_options["keyfile"], key);
	BOOST_REQUIRE_EQUAL(cmd.m_options["num"], num);
}

BOOST_AUTO_TEST_CASE(testServerComplete)
{
	const string server = "test";
	const char *argv[] = { "-s", server.c_str() };
	testCommandLineParser cmd(sizeof(argv) / sizeof(argv[0]), argv);
	BOOST_REQUIRE_EQUAL(cmd.m_options["server"], server + ":4433");
}

BOOST_AUTO_TEST_CASE(testCommands)
{
	const string command = "load";
	const char *argv[] = { "-n", "666", command.c_str() };
	testCommandLineParser cmd(sizeof(argv) / sizeof(argv[0]), argv);
	BOOST_REQUIRE_EQUAL(cmd.m_args.front(), command);
}

BOOST_AUTO_TEST_CASE(testLoadCreateHonest)
{
	const char *argv[] = { "-s", "test", "load" };
	struct inlineCommandLineParser : public testCommandLineParser, private visit_mock {
		inlineCommandLineParser(int argc, const char **argv) : testCommandLineParser(argc, argv) {};
		void manageClients(client_creator_t creator) const {
			visit();
			Client *honest = creator();
			BOOST_REQUIRE(dynamic_cast<ClientHonest *>(honest) != 0);
		}
	} cmd(sizeof(argv) / sizeof(argv[0]), argv);
	cmd.run(nullout);
}

BOOST_AUTO_TEST_CASE(testCreateManager)
{
	const char *argv[] = { "-n", "666", "load" };
	struct testCLP : public CommandLineParser, private visit_mock {
		testCLP(int argc, const char **argv) : CommandLineParser(argc, argv) {};
		ClientManager *createClientManager(client_creator_t, uint num) const {
			visit();
			BOOST_REQUIRE_EQUAL(num, uint(666));
			throw runtime_error("no need return");
		}
	} cmd(sizeof(argv) / sizeof(argv[0]), argv);
	BOOST_REQUIRE_THROW(cmd.run(nullout), runtime_error);
}

BOOST_AUTO_TEST_CASE(testCreateManagerDefaultSize)
{
	const char *argv[] = { "load" };
	struct testCLP : public CommandLineParser, private visit_mock {
		testCLP(int argc, const char **argv) : CommandLineParser(argc, argv) {};
		ClientManager *createClientManager(client_creator_t, uint num) const {
			visit();
			BOOST_REQUIRE_EQUAL(num, uint(1));
			throw runtime_error("no need return");
		}
	} cmd(sizeof(argv) / sizeof(argv[0]), argv);
	BOOST_REQUIRE_THROW(cmd.run(nullout), runtime_error);
}

BOOST_AUTO_TEST_CASE(testCreateManagerRunning)
{
	const char *argv[] = { "-s", "test", "load" };
	struct testCLP : public CommandLineParser, private visit_mock {
		testCLP(int argc, const char **argv) : CommandLineParser(argc, argv) {};
		ClientManager *createClientManager(client_creator_t creator, uint) const {
			visit();
			struct testClientManager : public ClientManager, private visit_mock {
				testClientManager(client_creator_t creator) : ClientManager(creator, 1) {}
				void run() const { visit(); };
			};
			return new testClientManager(creator);
		}
	} cmd(sizeof(argv) / sizeof(argv[0]), argv);
	BOOST_REQUIRE_NO_THROW(cmd.run(nullout));
}

BOOST_AUTO_TEST_CASE(testCreateClientHonest)
{
	testCommandLineParser parser;
	BOOST_REQUIRE_EXCEPTION(parser.createClientHonest(), runtime_error,
		check_exception_message("Необходимо указать опцию --server"));
}


BOOST_AUTO_TEST_SUITE_END();
