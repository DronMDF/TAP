
#pragma once

class Socket {
public:
	virtual ~Socket() {};
	virtual bool send(const std::vector<uint8_t> &data) = 0;
};
