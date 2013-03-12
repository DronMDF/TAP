
#pragma once
#include "ClientControl.h"

// Parent of load clients
class Client {
public:
	Client();
	virtual ~Client() = default;

	virtual void timeout(ClientControl *control) = 0;
	/// TapManager calls this method when the Сlient has no other events
	virtual void action(ClientControl *control);

	void setClientControl(const ClientControl &control);
		
protected:
	ClientControl control;

private:
	Client(const Client&) = delete;
	Client &operator=(const Client&) = delete;
};
