
#pragma once
#include <cstdint>
#include <vector>

class SocketHandler {
public:
	virtual ~SocketHandler();

	virtual void recv(const std::vector<uint8_t> &data);
	virtual std::vector<uint8_t> send();
};
