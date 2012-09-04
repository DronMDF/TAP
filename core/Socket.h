
#pragma once
#include <cstdint>
#include <vector>

class Socket {
public:
	virtual ~Socket() {};
	virtual bool send(const std::vector<uint8_t> &data) = 0;
};
