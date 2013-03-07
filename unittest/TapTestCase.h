
#pragma once
#include <boost/test/unit_test_suite.hpp>

// BOOST_AUTO_TEST_CASE use nil_t struct without virtual dtor for empty fixture, 
// This is not compatible with -Weffc++. We are write own TEST_CASE macro.

struct null_fixture { virtual ~null_fixture() {} };

#define TAP_TEST_CASE(name)	BOOST_FIXTURE_TEST_CASE(name, null_fixture)

