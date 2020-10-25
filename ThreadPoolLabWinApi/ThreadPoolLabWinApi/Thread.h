#pragma once
#include <windows.h>
#include <functional>

using namespace std;

class Thread {
public:
	Thread();
	~Thread();

	CRITICAL_SECTION criticalSection;
	CONDITION_VARIABLE conditionalVariable;
	function<void()> task;

	bool isRun;

};