#include <windows.h>
#include <iostream>

using namespace std;

TCHAR szName[] = TEXT("MyFileMappingObject");
CRITICAL_SECTION* crSection;

int main()
{
	HANDLE hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS, 
		FALSE,
		szName);
	if (hMapFile == NULL) {
		std::cout << "Cannot open shared memory critical section" << std::endl;
		return -1;
	}


	crSection = (CRITICAL_SECTION*)MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0, 
		0,
		sizeof(CRITICAL_SECTION));
	if (crSection == NULL) {
		std::cout << "Unable to map view critical section" << std::endl;
		CloseHandle(hMapFile);
		return -1;
	}

	//Критическая секция
	while (!TryEnterCriticalSection(crSection)) {
		Sleep(1000);
	}
	cout << GetCurrentProcessId() << "  -  in critical section now." << endl;
	Sleep(1000);
	LeaveCriticalSection(crSection);


	UnmapViewOfFile(crSection);
	CloseHandle(hMapFile);
	return 0;
}