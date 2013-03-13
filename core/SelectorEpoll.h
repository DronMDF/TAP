
#pragma once
#include <map>
#include "Selector.h"

class SelectorEpoll : public Selector {
public:
	SelectorEpoll();
	virtual ~SelectorEpoll() noexcept;

	virtual void addSocket(const std::shared_ptr<Socket> &socket) override;
	virtual void removeSocket(const std::shared_ptr<Socket> &socket) override;
	virtual void proceed() override;

private:
	int epollfd;
};

