
#pragma once
#include <functional>

// Абстрактный клиент нагрузки
class Client {
public:
	enum { OFFLINE, CONNECTING, ONLINE };
	
	Client();
	virtual ~Client();

	virtual int createMainDescriptor() = 0;
	virtual void readFromMain() = 0;
	
	void setStatsChanger(std::function<void (int, int)> state_changer);
	
	virtual void wakeup() = 0;
	
protected:
	void setState(int new_state);
	
private:
	std::function<void (int, int)> state_changer;
	int state;
};
