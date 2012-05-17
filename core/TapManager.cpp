
#include <chrono>
#include <iomanip>
#include <iostream>
#include <boost/foreach.hpp>
#include "Client.h"
#include "TapManager.h"
#include "Selector.h"

using namespace std;
using namespace std::placeholders;

TapManager::TapManager(unsigned nth, 
		       function<shared_ptr<Selector> (int)> create_selector,
		       function<shared_ptr<Client> ()> create_client)
	: main_ds(create_selector(nth)), extra_ds(create_selector(nth)), timeouts(nth, 0), 
	  clients(nth), stats()
{
	for (unsigned i = 0; i < nth; i++) {
		clients[i] = create_client();
		clients[i]->setStatsChanger(bind(&Statistic::changeState, &stats, _1, _2));
	}
}

void TapManager::showStatistics() const
{
	const auto ts = time(0);
	// TODO: Новый стандарт, пока не реализовано в gcc
	// cout << put_time(localtime(&ts), "%T")

	// TODO: Поэтому приходится мучаться
	char tsstr[100] = {0};
	if (strftime(tsstr, 100, "%T", localtime(&ts))) {
		cout << tsstr;
	} else {
		cout << ts;
	}

	cout	<< ": Offline: " << stats.offline
		<< ", Connecting: " << stats.connecting
		<< ", Online: " << stats.online << endl;
}

bool TapManager::selectAllFromMain(time_t deadline)
{
	while (true) {
		const int rc = main_ds->selectRead();
		if (rc == -1) {
			break;
		}
		
		try {
			clients[rc]->readFromMain();
		} catch (const std::exception &) {
			// Проблема с сокетом, пересоздать
			main_ds->setDescriptor(rc, clients[rc]->createMainDescriptor());
		}
		
		if (deadline < time(0)) {
			// Прерываемся на вывод статистики
			return false;
		}
	}
	
	return true;
}

bool TapManager::checkTimeouts(time_t deadline)
{
// 	for (unsigned i = 0; i < clients.size(); i++) {
// 		if (timeouts[i] < time(0)) {
// 			clients[i]->wakeup();
// 		}
// 
// 		if (deadline < time(0)) {
// 			// Прерываемся на вывод статистики
// 			return false;
// 		}
// 	}
	
	return true;
}

bool TapManager::selectAllToMain(time_t deadline)
{
// 	while (true) {
// 		const int wc = main_ds->selectWrite()
// 		if (wc == -1) {
//  			break;
// 		}
//
// 		int fd = main_ds->getDescriptor(wc);
// 		write(fd, data[wc].front());
// 		// Не зацикливаемся надолго, оставляем время для статистики.
// 
// 		if (deadline < time(0)) {
// 			// Прерываемся на вывод статистики
// 			return false;
// 		}
// 	}

	return true;
}

void TapManager::pressure()
{
	time_t status = 0;
	const int interval = 10;
	
	while (true) {
		const time_t now = time(0);
		
		if (status + interval < now) {
			showStatistics();
			status = now;
		}
		
		if (!selectAllFromMain(status + interval)) {
			continue;
		}
		
		if (!checkTimeouts(status + interval)) {
			continue;
		}
		
		if (!selectAllToMain(status + interval)) {
			continue;
		}
	}
}

list<unsigned> TapManager::selectIn()
{
	return {};
}

void TapManager::receive(unsigned i)
{
}
