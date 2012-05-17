
#pragma once
#include <functional>

class Tracer;
class ClientControl;

// Абстрактный клиент нагрузки
class Client {
public:
	enum { OFFLINE, CONNECTING, ONLINE };
	
	Client();
	virtual ~Client();

	virtual int createMainDescriptor(ClientControl *control) = 0;
	virtual void readFromMain(ClientControl *control) = 0;
	virtual void timeout(ClientControl *control) = 0;
	
	void setStatsChanger(std::function<void (int, int)> state_changer);
	void setTracer(Tracer *tracer);
	
	int getState() const;
	
protected:
	void setState(int new_state);
	
	Tracer *tracer;

private:
	Client(const Client&) = delete;
	Client &operator=(const Client&) = delete;
	
	std::function<void (int, int)> state_changer;
	int state;
	
	static Tracer null_tracer;
};
