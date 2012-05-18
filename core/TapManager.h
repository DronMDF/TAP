
#pragma once
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
	void writeToMain(unsigned n, const std::vector<uint8_t> &data);
	
	void pressure();
	
private:
	// Основной набор дескрипторов
	std::shared_ptr<Selector> main_ds;
	// Вспомогательный набор дескрипторов - открывается по инициативе клиентов.
	std::shared_ptr<Selector> extra_ds;
	// Список клиентов
	std::vector<std::shared_ptr<Client>> clients;
	// Очереди на отправку
	std::vector<std::queue<std::vector<uint8_t>>> queues;
	// Время реакции клиента
	std::vector<unsigned> timeouts;
	
	Statistic stats;
	
	void showStatistics() const;
	bool selectAllFromMain(time_t deadline);
	bool checkTimeouts(time_t deadline);
	bool selectAllToMain(time_t deadline);
};
