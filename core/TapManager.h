
#pragma once
#include <list>
#include <vector>
#include <memory>

class Client;
class ClientBuilder;
class Selector;

class TapManager {
public:
	TapManager(unsigned nth, std::function<std::shared_ptr<Selector> (int)> create_selector,
		const ClientBuilder &builder);
	virtual ~TapManager() {};
	
	void pressure();
	
private:
	// Основной набор дескрипторов - открывается при создании
	std::shared_ptr<Selector> main_ds;
	// Вспомогательный набор дескрипторов - открывается по инициативе клиентов.
	std::shared_ptr<Selector> extra_ds;
	// Время реакции клиента
	std::vector<unsigned> timeouts;
	// Список клиентов
	std::vector<std::shared_ptr<Client>> clients;
	
	virtual std::list<unsigned> selectIn();
	virtual void receive(unsigned i);
	
	bool selectAllFromMain(time_t deadline);
	bool checkTimeouts(time_t deadline);
	bool selectAllToMain(time_t deadline);
};
