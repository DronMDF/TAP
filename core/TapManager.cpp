
#include <assert.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <set>
#include <boost/foreach.hpp>
#include "Client.h"
#include "ClientControl.h"
#include "Selector.h"
#include "TapManager.h"

using namespace std;
using namespace std::placeholders;

TapManager::TapManager(unsigned nth, 
		       function<shared_ptr<Selector> (int)> create_selector,
		       function<shared_ptr<Client> ()> create_client)
	: main_ds(create_selector(nth)), extra_ds(create_selector(nth)), 
	  clients(nth), queues(nth), timeouts(nth, 0), stats()
{
	for (unsigned i = 0; i < nth; i++) {
		clients[i] = create_client();
		clients[i]->setStatsChanger(bind(&Statistic::changeState, &stats, _1, _2));
	}
}

void TapManager::setTracer(unsigned n, Tracer *tracer)
{
	assert(n < clients.size());
	clients[n]->setTracer(tracer);
}

void TapManager::setTimeout(unsigned n, unsigned timeout)
{
	assert(n < clients.size());
	timeouts[n] = time(0) + timeout;
}

void TapManager::writeToMain(unsigned n, const std::vector<uint8_t> &data)
{
	assert(n < clients.size());
	queues[n].push(data);
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
		
		ClientControl control(this, rc);
		try {
			clients[rc]->readFromMain(&control);
		} catch (const std::exception &) {
			// Проблема с сокетом, пересоздать
			main_ds->setDescriptor(rc, clients[rc]->createMainDescriptor(&control));
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
	for (unsigned i = 0; i < clients.size(); i++) {
		if (timeouts[i] < time(0)) {
			ClientControl control(this, i);
			clients[i]->timeout(&control);
		}

		if (deadline < time(0)) {
			// Прерываемся на вывод статистики
			return false;
		}
	}
	
	return true;
}

bool TapManager::selectAllToMain(time_t deadline)
{
	set<unsigned> intrest;
	for (unsigned i = 0; i < queues.size(); i++) {
		if (!queues[i].empty()) {
			intrest.insert(i);
		}
	}
	
	if (intrest.empty()) {
		return true;
	}
	
	while (true) {
		// Не зацикливаемся надолго, оставляем время для статистики.
		if (deadline < time(0)) {
			// Прерываемся на вывод статистики
			return false;
		}
		
		int rv = main_ds->selectWrite(intrest);
		if (rv == -1) {
			return true;
		}
		// Indexes is not need in the future
		intrest.clear();
		
		int fd = main_ds->getDescriptor(rv);
		vector<uint8_t> data = queues[rv].front();
		queues[rv].pop();
		
		if (write(fd, &data[0], data.size()) != int(data.size())) {
			// Проблема с сокетом, пересоздать
			ClientControl control(this, rv);
			main_ds->setDescriptor(rv, clients[rv]->createMainDescriptor(&control));
		}
	}

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
		
		//cout << "Пытаемся протаймаутить кого нибудь..." << endl;
		if (!checkTimeouts(status + interval)) {
			continue;
		}
		
		// cout << "Пытаемся записать что нибудь..." << endl;
		if (!selectAllToMain(status + interval)) {
			continue;
		}
		
		// Попытаемся создать недостающих
		for (unsigned i = 0; i < clients.size(); i++) {
			if (main_ds->getDescriptor(i) == -1) {
				ClientControl control(this, i);
				main_ds->setDescriptor(i, clients[i]->createMainDescriptor(&control));
			}
			
			// Не зацикливаемся надолго, оставляем время для статистики.
			if (status + interval < time(0)) {
				// Прерываемся на вывод статистики
				break;
			}
		}
		
		// sleep(1);
	}
}
