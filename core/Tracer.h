
#pragma once
#include <string>

// ��� ������������ ��������� � ������ �������
class Tracer {
public:
	virtual ~Tracer();
	virtual void trace(const std::string &msg) const;
};
