
#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include "TimePoint.h"

class Socket;
class TapManager;
class Tracer;

class ClientControl {
public:
	ClientControl(TapManager *tapm, unsigned n);
	ClientControl &operator = (const ClientControl &c);

	void addSocket(const std::shared_ptr<Socket> &socket) const;
	/// Removing live socket from poll (dead socket removed automatically)
	void removeSocket(const std::shared_ptr<Socket> &socket) const;

	void writeToMain(const std::vector<uint8_t> &data) const;
	void setWakeupTime(const time_point &wakeup_time) const;
	
	void setStateOffline() const;
	void setStateConnecting() const;
	void setStateOnline() const;
	
	void trace(const std::string &message) const;
	void trace(const std::string &key, unsigned value) const;
	
private:
	TapManager *tapm;
	unsigned n;
};
