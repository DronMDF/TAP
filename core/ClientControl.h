
#pragma once
#include <vector>
#include <cstdint>

class TapManager;

class ClientControl {
public:
	ClientControl(TapManager *tapm, unsigned n);

	void setMainDescriptor(int fd) const;
	void writeToMain(const std::vector<uint8_t> &data) const;
	void setTimeout(unsigned timeout) const;
	
private:
	TapManager *tapm;
	unsigned n;
};
