
#pragma once
#include <list>
#include <vector>
#include <memory>
#include "Statistic.h"

class Client;
class Selector;

class TapManager {
public:
	TapManager(unsigned nth, 
		   std::function<std::shared_ptr<Selector>(int)> create_selector,
		   std::function<std::shared_ptr<Client>()> create_client);
	
	virtual ~TapManager() {};
	
	void pressure();
	
private:
	// �������� ����� ������������
	std::shared_ptr<Selector> main_ds;
	// ��������������� ����� ������������ - ����������� �� ���������� ��������.
	std::shared_ptr<Selector> extra_ds;
	// ����� ������� �������
	std::vector<unsigned> timeouts;
	// ������ ��������
	std::vector<std::shared_ptr<Client>> clients;
	
	Statistic stats;
	
	virtual std::list<unsigned> selectIn();
	virtual void receive(unsigned i);
	
	void showStatistics() const;
	bool selectAllFromMain(time_t deadline);
	bool checkTimeouts(time_t deadline);
	bool selectAllToMain(time_t deadline);
};
