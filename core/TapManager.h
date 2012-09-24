
#pragma once
#include <list>
#include <memory>
#include <queue>
#include <vector>
#include "TimePoint.h"

class Client;
class Selector;
class Socket;
class Tracer;

class TapManager {
public:
	TapManager(unsigned nth, 
		   std::function<std::shared_ptr<Selector>(int)> create_selector,
		   std::function<std::shared_ptr<Client>()> create_client);
	
	virtual ~TapManager() {};
	
	void setTracer(unsigned n, Tracer *tracer);
	void setTimeout(unsigned n, const time_point &wakeup_time);
	
	virtual void setStateOffline(unsigned n);
	virtual void setStateConnecting(unsigned n);
	virtual void setStateOnline(unsigned n);

	void setSocket(unsigned n, const std::shared_ptr<const Socket> &socket);
	void setMainDescriptor(unsigned n, int fd);
	
	void writeToMain(unsigned n, const std::vector<uint8_t> &data);
	
	void setShowStatistic(std::function<void (int, int, int)> show_statistic, 
		const std::chrono::seconds &interval);
	
	void pressure();
	
private:
	std::shared_ptr<Selector> main_ds;
	std::vector<std::shared_ptr<Client>> clients;
	std::vector<std::queue<std::vector<uint8_t>>> queues;
	std::vector<time_point> timeouts;
	
	static Tracer nulltracer;
	std::vector<Tracer *> tracers;
	
	enum { OFFLINE, CONNECTING, ONLINE };
	std::vector<int> clients_states;
	std::function<void (int, int, int)> show_statistic;
	time_point stats_time;
	std::chrono::seconds stats_interval;
	
	unsigned action_idx;

	void showStatistics();
	void selectAllFromMain();
	void checkTimeouts(const time_point &now);
	void selectAllToMain();
	bool needToAction(const time_point &endtime);
};

