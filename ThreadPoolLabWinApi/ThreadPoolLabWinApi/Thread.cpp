#include "Thread.h"

Thread::Thread()
{
	InitializeConditionVariable(&conditionalVariable);
	InitializeCriticalSection(&criticalSection);
	task= NULL;
}


Thread::~Thread()
{
	DeleteCriticalSection(&criticalSection);
}
