
#pragma once
#include "Selector.h"

class SelectorPoll : public Selector {
public:
	SelectorPoll();
	SelectorPoll(int n);

	virtual void proceed();
};
