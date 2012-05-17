
#pragma once
#include <list>
#include <vector>
#include <memory>
#include "Statistic.h"

class Client;
class Selector;

class TapManager {
public:
	TapManager(unsigned nth, 
		   std::function<std::shared_ptr<Selector>(int)> create_selector,
		   std::function<std::shared_ptr<Client>()> create_client);
	
	virtual ~TapManager() {};
	
	void pressure();
	
private:
	// Основной набор дескрипторов
	std::shared_ptr<Selector> main_ds;
	// Вспомогательный набор дескрипторов - открывается по инициативе клиентов.
	std::shared_ptr<Selector> extra_ds;
	// Время реакции клиента
	std::vector<unsigned> timeouts;
	// Список клиентов
	std::vector<std::shared_ptr<Client>> clients;
	
	Statistic stats;
	
	virtual std::list<unsigned> selectIn();
	virtual void receive(unsigned i);
	
	void showStatistics() const;
	bool selectAllFromMain(time_t deadline);
	bool checkTimeouts(time_t deadline);
	bool selectAllToMain(time_t deadline);
};
