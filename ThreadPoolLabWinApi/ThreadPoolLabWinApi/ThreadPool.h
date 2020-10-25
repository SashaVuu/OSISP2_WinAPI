#pragma once

#include <windows.h>
#include <functional>
#include <queue>
#include "Logger.h"
#include "Thread.h"


using namespace std;


class ThreadPool {
public:
	ThreadPool(int amountOfThreads);
	void AddTask(function<void()> func);
	~ThreadPool();

private:

	const int MAX_THREAD_AMOUNT = 10;
	int currentThreadAmount;
	LONG currentWorkingThreadAmount;


	Logger* logger;
	HANDLE managerThread;
	LONG isManagerAlive;
	bool isPoolAlive;
	HANDLE* threads;

	queue<Thread*> threadsQueue;
	queue<function<void()>> taskQueue;


	CRITICAL_SECTION taskQueueCrSection;
	CONDITION_VARIABLE taskQueueCnVariable;


	CRITICAL_SECTION threadQueueCrSection;
	CONDITION_VARIABLE threadQueueCnVariable;

	static DWORD WINAPI startThread(ThreadPool* tp);
	static DWORD WINAPI startManager(ThreadPool* tp);

	bool WaitForAllThreads();
	bool WaitForOneThread();


};

