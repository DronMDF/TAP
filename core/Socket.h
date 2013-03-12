
#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

class Socket {
public:
	virtual ~Socket() = default;

	virtual int getDescriptor() const = 0;

	virtual bool recv() = 0;
	virtual bool send() = 0;
};
