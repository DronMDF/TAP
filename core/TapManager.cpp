
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
	  timeouts(nth, time_point::max()), tracers(nth, &nulltracer), stats(),
	  clients_states(nth, OFFLINE), show_statistic([](int, int, int){}), 
	  stats_time(time_point::max()), stats_interval(std::chrono::seconds::max())
{
	for (auto &client: clients) {
		client = create_client();
	}
}

void TapManager::setTracer(unsigned n, Tracer *tracer)
{
	assert(n < clients.size());
	tracers[n] = tracer;
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

void TapManager::setStateOffline(unsigned n)
{
	assert(n < clients.size());
	clients_states[n] = OFFLINE;
}

void TapManager::setStateConnecting(unsigned n)
{
	assert(n < clients.size());
	clients_states[n] = CONNECTING;
}

void TapManager::setStateOnline(unsigned n)
{
	assert(n < clients.size());
	clients_states[n] = ONLINE;
}

void TapManager::writeToMain(unsigned n, const vector<uint8_t> &data)
{
	assert(n < clients.size());
	queues[n].push(data);
}

void TapManager::setShowStatistic(function<void (int, int, int)> show, 
	const chrono::seconds &interval)
{
	this->show_statistic = show;
	stats_time = time_point::min();
	stats_interval = interval;
}

void TapManager::showStatistics()
{
	const auto now = chrono::high_resolution_clock::now();
	if (now < stats_time) {
		return;
	}
	
	int offline = 0;
	int connecting = 0;
	int online = 0;
	for (auto i: clients_states) {
		switch (i) {
			case OFFLINE: offline++; break;
			case CONNECTING: connecting++; break;
			case ONLINE: online++; break;
			default: assert(i);
		}
	}

	show_statistic(offline, connecting, online);
	stats_time = chrono::high_resolution_clock::now() + stats_interval;
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
		
		showStatistics();
		
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

