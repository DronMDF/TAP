
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
	: selector(create_selector(nth)), clients(nth), queues(nth), 
	  timeouts(nth, time_point::max()), tracers(nth, &nulltracer),
	  clients_states(nth, OFFLINE), show_statistic([](int, int, int){}), 
	  stats_time(time_point::max()), stats_interval(std::chrono::seconds::max()),
	  action_idx(0)
{
	for (unsigned i = 0; i < clients.size(); i++) {
		clients[i] = create_client();
		clients[i]->setClientControl(ClientControl(this, i));
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

void TapManager::setSocket(const std::shared_ptr<Socket> &socket)
{
	selector->addSocket(socket);
}

void TapManager::removeSocket(const std::shared_ptr<Socket> &socket)
{
	selector->removeSocket(socket);
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

void TapManager::trace(unsigned n, const string &message)
{
	assert(n < clients.size());
	tracers[n]->trace(n, message);
}

void TapManager::trace(unsigned n, const string &key, unsigned value)
{
	assert(n < clients.size());
	tracers[n]->trace(n, key, value);
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

void TapManager::checkTimeouts(const time_point &now)
{
	for (unsigned i = 0; i < clients.size(); i++) {
		if (timeouts[i] < now) {
			ClientControl control(this, i);
			clients[i]->timeout(&control);
		}
	}
}

bool TapManager::needToAction(const time_point &endtime)
{
	for (; action_idx < clients.size(); action_idx++) {
		ClientControl control(this, action_idx);
		clients[action_idx]->action(&control);
		
		if (chrono::high_resolution_clock::now() > endtime) {
			return false;
		}
	}
	
	action_idx = 0;
	return true;
}

void TapManager::pressure()
{
	while (true) {
		const auto now = chrono::high_resolution_clock::now();
		const auto endtime = now + chrono::seconds(10);
		
		showStatistics();
		
		selector->proceed();

		checkTimeouts(chrono::high_resolution_clock::now());

		if (chrono::high_resolution_clock::now() > endtime) {
			cout << "break on phase 1" << endl;
			continue;
		}
		
		if (!needToAction(endtime)) {
			cout << "break on action" << endl;
			continue;
		}
		
		const auto interval = chrono::high_resolution_clock::now() - now;
		if (interval > chrono::seconds(1)) {
			cout << "Long cycle time: " << chrono::duration_cast<chrono::milliseconds>(interval).count() << endl;
		}

		sched_yield();
	}
}

