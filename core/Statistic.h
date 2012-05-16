
#pragma once

class Statistic {
public:
	Statistic();
	
	void changeState(int from, int to);

	int offline;
	int connecting;
	int online;
};
