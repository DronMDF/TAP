
#pragma once
#include <chrono>
#include <cstdint>
#include <vector>

class TapManager;

class ClientControl {
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
public:
	ClientControl(TapManager *tapm, unsigned n);

	void setMainDescriptor(int fd) const;
	void writeToMain(const std::vector<uint8_t> &data) const;
	void setWakeupTime(const time_point &wakeup_time) const;
	
private:
	TapManager *tapm;
	unsigned n;
};
