
#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

class Socket {
public:
	virtual ~Socket() = default;

	virtual int getDescriptor() const = 0;

	virtual int recv(int size) = 0;
	virtual int send(int size) = 0;
};
