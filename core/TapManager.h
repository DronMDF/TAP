
#pragma once
#include <list>
#include <memory>
#include <queue>
#include <vector>
#include "Statistic.h"
#include "TimePoint.h"

class Client;
class Selector;
class Tracer;

class TapManager {
public:
	TapManager(unsigned nth, 
		   std::function<std::shared_ptr<Selector>(int)> create_selector,
		   std::function<std::shared_ptr<Client>()> create_client);
	
	virtual ~TapManager() {};
	
	void setTracer(unsigned n, Tracer *tracer);
	void setTimeout(unsigned n, const time_point &wakeup_time);

	void setMainDescriptor(unsigned n, int fd);
	
	void writeToMain(unsigned n, const std::vector<uint8_t> &data);
	
	void pressure();
	
private:
	std::shared_ptr<Selector> main_ds;
	std::vector<std::shared_ptr<Client>> clients;
	std::vector<std::queue<std::vector<uint8_t>>> queues;
	std::vector<time_point> timeouts;
	
	Statistic stats;
	
	void showStatistics() const;
	bool selectAllFromMain(const time_point &endtime);
	bool checkTimeouts(const time_point &endtime);
	bool selectAllToMain(const time_point &endtime);
	bool needToAction(const time_point &endtime);
};

