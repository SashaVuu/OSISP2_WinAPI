#include "Logger.h"
#include <string>

Logger::Logger(string path)
{
	InitializeCriticalSection(&criticalSection);
	file.open(path);
}

Logger::~Logger()
{
	file.close();
	DeleteCriticalSection(&criticalSection);
}

void Logger::Write(string info, DWORD id)
{
	EnterCriticalSection(&criticalSection);
	int id_int = (int)id;
	string id_s= to_string(id_int);

	if (file.is_open()) 
	{
		file << id_s+ " "+ info  << endl;
	}
	LeaveCriticalSection(&criticalSection);
}

