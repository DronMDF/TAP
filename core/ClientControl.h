
#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "TimePoint.h"

class TapManager;
class Tracer;

class ClientControl {
public:
	ClientControl(TapManager *tapm, unsigned n, Tracer *tracer);

	void setMainDescriptor(int fd) const;
	void writeToMain(const std::vector<uint8_t> &data) const;
	void setWakeupTime(const time_point &wakeup_time) const;
	void setState(int state) const;
	
	void trace(const std::string &message) const;
	void trace(const std::string &key, unsigned value) const;
	
private:
	TapManager *tapm;
	unsigned n;
	Tracer *tracer;
};
