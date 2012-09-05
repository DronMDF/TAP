
#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

class Socket {
public:
	virtual ~Socket() {};

	virtual std::vector<uint8_t> recv() = 0;
	virtual size_t send(const std::vector<uint8_t> &data) = 0;
};
