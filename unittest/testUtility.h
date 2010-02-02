#pragma once

#include <iostream>
#include <string>
#include <stdexcept>

class check_exception_message {
private:
	const std::string &m_message;

public:
	check_exception_message(const std::string &msg)
		: m_message(msg)
	{ }

	bool operator() (const std::exception &exc) const {
		if (exc.what() == m_message)
			return true;

		std::cerr << "Exception message differ: " << std::endl;
		std::cerr << "\texpected: '" << m_message << '\'' << std::endl;
		std::cerr << "\tcatched: '" << exc.what() << '\'' << std::endl;
		return false;
	}
};

template<int callcount>
class order_mock {
private:
	mutable int m_order;
public:
	explicit order_mock() : m_order(0) {}
	~order_mock() { BOOST_CHECK_EQUAL(m_order, callcount); }

	void order_next() const { ++m_order; }
	void order(int n) const { BOOST_REQUIRE_EQUAL(++m_order, n); }
};

class visit_mock : private order_mock<1> {
public:
	void visit() const { order(1); }
};


