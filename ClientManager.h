
#pragma once

#include <list>
#include <boost/shared_ptr.hpp>

#include "Client.h"

class ClientManager {
public:
	typedef std::list<boost::shared_ptr<Client> > clients_list_t;

protected:
	clients_list_t m_clients;
	
public:
	ClientManager(client_creator_t creator, uint num);
	virtual ~ClientManager() {}
	void run() const;
};
