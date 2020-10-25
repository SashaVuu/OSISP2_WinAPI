#pragma once
#include <windows.h>
#include <fstream>

using namespace std;

class Logger {
public:
	Logger(string path);
	~Logger();
	void Write(string log,DWORD threadId);
private:
	std::ofstream file;
	CRITICAL_SECTION criticalSection;
};