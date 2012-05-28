
#pragma once
#include <functional>
#include <vector>

class Tracer;
class ClientControl;

// Абстрактный клиент нагрузки
class Client {
public:
	enum { OFFLINE, CONNECTING, ONLINE };
	
	Client();
	virtual ~Client();

	virtual void readFromMain(ClientControl *control) = 0;
	virtual bool writeToMain(ClientControl *control, const std::vector<uint8_t> &data);
	virtual void timeout(ClientControl *control) = 0;
	
	/// TapManager calls this method when the Сlient has no other events
	virtual void action(ClientControl *control);
		
	void setStatsChanger(std::function<void (int, int)> state_changer);
	void setTracer(Tracer *tracer);
	
	int getState() const;
	
protected:
	void setState(int new_state);
	
	Tracer *tracer;

private:
	Client(const Client&) = delete;
	Client &operator=(const Client&) = delete;
	
	virtual int getMain() const = 0;
	
	std::function<void (int, int)> state_changer;
	int state;
	
	static Tracer null_tracer;
};
