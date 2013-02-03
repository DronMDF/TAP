
#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

class Socket {
public:
	virtual ~Socket() {};

	virtual int getDescriptor() const = 0;

	virtual std::vector<uint8_t> recv(size_t size) = 0;
	virtual size_t send(const std::vector<uint8_t> &data) = 0;

	// New API
	virtual void recv() = 0;
	virtual void send() = 0;
};
