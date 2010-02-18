
#pragma once

#include <list>
#include <boost/shared_ptr.hpp>

#include "Client.h"

class Logger;

class ClientManager {
private:
	Logger *m_logger;

	ClientManager(const ClientManager &);
	ClientManager &operator =(const ClientManager &);

public:
	typedef std::list<boost::shared_ptr<Client> > clients_list_t;

protected:
	clients_list_t m_clients;
	
public:
	ClientManager(client_creator_t creator, uint num);
	virtual ~ClientManager() {}
	virtual void run() const;

	void setLogger(Logger *log);
	
	virtual void eventIteration() const;
};
