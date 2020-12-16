#include <iostream>
#include <windows.h>

using namespace std;

STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
PROCESS_INFORMATION proccesInfo;


int main()
{
 
    LPCWSTR appName = L".\\TestApp.exe";
    char dllName[] = ".\\HookSysCallsLib.dll";

    wstring CommandLine(L"TestApp.exe");
    LPWSTR lpCommandLine = &CommandLine[0];

    ZeroMemory(&proccesInfo, sizeof(proccesInfo));
    startupInfo.cb = sizeof(startupInfo);


    CreateProcess(NULL, lpCommandLine, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startupInfo, &proccesInfo);
    HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, proccesInfo.dwProcessId);

    FARPROC LoadLibraryAddress = GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryA");


    PVOID rmMemory = VirtualAllocEx(process, NULL, strlen(dllName) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    WriteProcessMemory(process, (LPVOID)rmMemory, dllName, strlen(dllName) + 1, NULL);

    HANDLE remoteThread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, (LPVOID)rmMemory, NULL, NULL);
  
    ResumeThread(proccesInfo.hThread);

    WaitForSingleObject(proccesInfo.hProcess, INFINITE);

    CloseHandle(proccesInfo.hProcess);
    CloseHandle(proccesInfo.hThread);

    return 0;
}

