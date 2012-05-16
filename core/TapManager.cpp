
#include <iostream>
#include <boost/foreach.hpp>
#include "Client.h"
#include "ClientBuilder.h"
#include "TapManager.h"
#include "Selector.h"

using namespace std;

TapManager::TapManager(unsigned nth, 
		       function<shared_ptr<Selector> (int)> create_selector,
		       function<shared_ptr<Client> ()> create_client)
	: main_ds(create_selector(nth)), extra_ds(create_selector(nth)), timeouts(nth, 0), clients(nth)
{
	for (unsigned i = 0; i < nth; i++) {
		clients[i] = create_client();
	}
}


bool TapManager::selectAllFromMain(time_t deadline)
{
//	while (true) {
//		const int rc = main_ds->selectRead();
//		if (rc == -1) {
//			break;
//		}
//		
//		try {
//			client[rc]->readFromMain();
//		} catch (const std::exception &) {
//			// Проблема с сокетом, пересоздать
//			main_ds.setDescriptor(rc, client[rc]->createMainDescriptor());
//		}
//		
//		if (deadline < time(0)) {
//			// Прерываемся на вывод статистики
//			return false;
//		}
//	}
	
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
			cout << "Offline: " << /*offline*/0
				<< ", Connecting: " << /*connecting*/0
				<< ", Online: " << /*online*/0 << endl;
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
