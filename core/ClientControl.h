
#pragma once
#include <cstdint>
#include <vector>
#include "TimePoint.h"

class TapManager;

class ClientControl {
public:
	ClientControl(TapManager *tapm, unsigned n);

	void setMainDescriptor(int fd) const;
	void writeToMain(const std::vector<uint8_t> &data) const;
	void setWakeupTime(const time_point &wakeup_time) const;
	
private:
	TapManager *tapm;
	unsigned n;
};
