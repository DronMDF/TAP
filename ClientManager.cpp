
#include <boost/foreach.hpp>
#include "ClientManager.h"
#include "Logger.h"

// Клиент менеджер очень примитивен. он не знает что за клиенты у него в
// подчинении, как они создаются и чем занимаются. Единственная его задача -
// построить указанное количество клиентов и дегать их в произвольном порядке.
// Может быть на сигналы заставим его реагировать - ну типа прерываться...
// видно будет

ClientManager::ClientManager(client_creator_t creator, uint num)
	: m_logger(0), m_clients()
{
	while (m_clients.size() < num) {
		m_clients.push_back(clients_list_t::value_type(creator()));
	}
}

void ClientManager::run() const
{
	while (true) {
		eventIteration();
		BOOST_FOREACH(const clients_list_t::value_type &client, m_clients) {
			client->push();
		}
	}
}

void ClientManager::setLogger(Logger *log)
{
	m_logger = log;
}

void ClientManager::eventIteration() const
{
	if (m_logger != 0) m_logger->logIteration();
}
