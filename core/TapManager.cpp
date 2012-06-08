
#include "TapManager.h"

#include <assert.h>
#include <sched.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <set>
#include <boost/foreach.hpp>
#include "Client.h"
#include "ClientControl.h"
#include "Selector.h"
#include "Tracer.h"

using namespace std;
using namespace std::placeholders;


Tracer TapManager::nulltracer;

TapManager::TapManager(unsigned nth, 
		       function<shared_ptr<Selector> (int)> create_selector,
		       function<shared_ptr<Client> ()> create_client)
	: main_ds(create_selector(nth)), clients(nth), queues(nth), 
	  timeouts(nth, time_point::max()), tracers(nth, &nulltracer), stats()
{
	for (unsigned i = 0; i < nth; i++) {
		clients[i] = create_client();
		clients[i]->setStatsChanger(bind(&Statistic::changeState, &stats, _1, _2));
	}
}

void TapManager::setTracer(unsigned n, Tracer *tracer)
{
	assert(n < clients.size());
	tracers[n] = tracer;
	clients[n]->setTracer(tracer);
}

void TapManager::setTimeout(unsigned n, const time_point &wakeup_time)
{
	assert(n < clients.size());
	timeouts[n] = wakeup_time;
}

void TapManager::setMainDescriptor(unsigned n, int fd)
{
	assert(n < clients.size());
	main_ds->setDescriptor(n, fd);
}

void TapManager::writeToMain(unsigned n, const vector<uint8_t> &data)
{
	assert(n < clients.size());
	queues[n].push(data);
}

void TapManager::showStatistics() const
{
	const auto ts = time(0);
	// TODO: Not implement in gcc-4.5
	// cout << put_time(localtime(&ts), "%T")

	// TODO: old style formatter
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

bool TapManager::selectAllFromMain(const time_point &endtime)
{
	while (true) {
		const int rc = main_ds->selectRead();
		if (rc == -1) {
			break;
		}
		
		ClientControl control(this, rc, tracers[rc]);
		clients[rc]->readFromMain(&control);
		
		if (chrono::high_resolution_clock::now() > endtime) {
			return false;
		}
	}
	
	return true;
}

bool TapManager::checkTimeouts(const time_point &endtime)
{
	for (unsigned i = 0; i < clients.size(); i++) {
		const auto now = chrono::high_resolution_clock::now();
		if (now > endtime) {
			return false;
		}
		
		if (timeouts[i] < now) {
			ClientControl control(this, i, tracers[i]);
			clients[i]->timeout(&control);
		}
	}
	
	return true;
}

bool TapManager::selectAllToMain(const time_point &endtime)
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
		if (chrono::high_resolution_clock::now() > endtime) {
			return false;
		}
		
		int rv = main_ds->selectWrite(intrest);
		if (rv == -1) {
			return true;
		}
		
		// Indexes is not need in the future
		intrest.clear();
	
		ClientControl control(this, rv, tracers[rv]);
		if (clients[rv]->writeToMain(&control, queues[rv].front())) {
			// Remove if success
			queues[rv].pop();
		}
	}

	return true;
}

bool TapManager::needToAction(const time_point &endtime)
{
	for (unsigned i = 0; i < clients.size(); i++) {
		ClientControl control(this, i, tracers[i]);
		clients[i]->action(&control);
		
		if (chrono::high_resolution_clock::now() > endtime) {
			return false;
		}
	}
	
	return true;
}

void TapManager::pressure()
{
	time_point status;
	
	while (true) {
		const auto now = chrono::high_resolution_clock::now();
		const auto endtime = now + chrono::seconds(1);
		
		if (now > status) {
			showStatistics();
			status = now + chrono::seconds(10);
		}
		
		if (!selectAllFromMain(endtime)) {
			cout << "break on read" << endl;
			continue;
		}
		
		if (!checkTimeouts(endtime)) {
			cout << "break on check timeout" << endl;
			continue;
		}
		
		if (!selectAllToMain(endtime)) {
			cout << "break on write" << endl;
			continue;
		}
		
		if (!needToAction(endtime)) {
			cout << "break on action" << endl;
			continue;
		}
		
		sched_yield();
	}
}

