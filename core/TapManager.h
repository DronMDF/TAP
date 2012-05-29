
#pragma once
#include <chrono>
#include <list>
#include <memory>
#include <queue>
#include <vector>
#include "Statistic.h"

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
	void setTimeout(unsigned n, unsigned timeout);

	void setMainDescriptor(unsigned n, int fd);
	
	void writeToMain(unsigned n, const std::vector<uint8_t> &data);
	
	void pressure();
	
private:
	std::shared_ptr<Selector> main_ds;
	std::vector<std::shared_ptr<Client>> clients;
	std::vector<std::queue<std::vector<uint8_t>>> queues;
	std::vector<time_t> timeouts;
	
	Statistic stats;
	
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
	
	void showStatistics() const;
	bool selectAllFromMain(const time_point &endtime);
	bool checkTimeouts(const time_point &endtime);
	bool selectAllToMain(const time_point &endtime);
	bool needToAction(const time_point &endtime);
};

