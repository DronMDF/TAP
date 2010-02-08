
#include "ClientManager.h"

// Клиент менеджер очень примитивен. он не знает что за клиенты у него в
// подчинении, как они создаются и чем занимаются. Единственная его задача -
// построить указанное количество клиентов и дегать их в произвольном порядке.
// Может быть на сигналы заставим его реагировать - ну типа прерываться...
// видно будет

ClientManager::ClientManager(client_creator_t creator, uint num)
	: m_clients()
{
	while (m_clients.size() < num) {
		m_clients.push_back(clients_list_t::value_type(creator()));
	}
}

void ClientManager::run() const
{
}
