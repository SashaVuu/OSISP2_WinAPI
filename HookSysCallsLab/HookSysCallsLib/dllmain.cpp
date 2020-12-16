// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "detours.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#pragma comment(lib,"detours.lib")

using namespace std;

/*FileSystem*/

//1.WriteFile
//2.SetFilePointer
//3.ReadFile
//4.DeleteFile


/*Registry*/
//6.RegOpenKey
//7.RegCloseKey
//8.RegCreateKey
//9.RegDeleteKey

HANDLE hStdOut;         //Консоль

VOID displayMessage(wstring);


//Filesys

HANDLE(WINAPI* pCreateFile) (LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = CreateFile;
HANDLE WINAPI hookCreateFile(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

BOOL(WINAPI* pReadFile) (HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED) = ReadFile;
BOOL WINAPI hookReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);


DWORD(WINAPI* pSetFilePointer) (HANDLE, LONG, PLONG, DWORD) = SetFilePointer;
DWORD WINAPI hookSetFilePointer(HANDLE, LONG, PLONG, DWORD);


BOOL(WINAPI* pDeleteFile) (LPCWSTR) = DeleteFile;
BOOL WINAPI hookDeleteFile(LPCWSTR);


//Registry
LONG(WINAPI* pRegOpenKey) (HKEY, LPCWSTR, PHKEY) = RegOpenKey;
LONG WINAPI hookRegOpenKey(HKEY, LPCWSTR, PHKEY);


LSTATUS(WINAPI* pRegCloseKey) (HKEY) = RegCloseKey;
LSTATUS WINAPI hookRegCloseKey(HKEY);

LSTATUS(WINAPI* pRegDeleteKey) (HKEY hKey, LPCWSTR lpSubKey) = RegDeleteKey;
LSTATUS WINAPI hookRegDeleteKey(HKEY, LPCWSTR);


LSTATUS(WINAPI* pRegCreateKey) (HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult) = RegCreateKey;
LSTATUS WINAPI hookRegCreateKey(HKEY, LPCWSTR, PHKEY);


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

        
        DisableThreadLibraryCalls(hModule);
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach(&(PVOID&)pCreateFile, hookCreateFile);
        DetourAttach(&(PVOID&)pReadFile, hookReadFile);
        DetourAttach(&(PVOID&)pSetFilePointer, hookSetFilePointer);
        DetourAttach(&(PVOID&)pDeleteFile, hookDeleteFile);

        DetourAttach(&(PVOID&)pRegOpenKey, hookRegOpenKey);
        DetourAttach(&(PVOID&)pRegCloseKey, hookRegCloseKey);
        DetourAttach(&(PVOID&)pRegCreateKey, hookRegCreateKey);
        DetourAttach(&(PVOID&)pRegCreateKey, hookRegDeleteKey);

        DetourTransactionCommit();
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourDetach(&(PVOID&)pCreateFile, hookCreateFile);
        DetourDetach(&(PVOID&)pReadFile, hookReadFile);
        DetourDetach(&(PVOID&)pSetFilePointer, hookSetFilePointer);
        DetourDetach(&(PVOID&)pDeleteFile, hookDeleteFile);


        DetourDetach(&(PVOID&)pRegOpenKey, hookRegOpenKey);
        DetourDetach(&(PVOID&)pRegCloseKey, hookRegCloseKey);
        DetourDetach(&(PVOID&)pRegCreateKey, hookRegCreateKey);
        DetourDetach(&(PVOID&)pRegCreateKey, hookRegDeleteKey);


        DetourTransactionCommit();

        CloseHandle(hStdOut);
        break;
    }
    return TRUE;
}


DWORD WINAPI hookSetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod) {

    wostringstream message;
    message << L"SetFilePointer() was called. To position: " << lDistanceToMove << L"\n";
    displayMessage(message.str());
    return pSetFilePointer(hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
}


HANDLE WINAPI hookCreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
   
    wostringstream message;
    message << L"CreateFile() was called. File name: " << (wstring)lpFileName << L"\n";
    displayMessage(message.str());
    return pCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

}

BOOL WINAPI hookReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
    wostringstream message;
    message << L"ReadFile() was called. Bytes to read: " << nNumberOfBytesToRead << L"\n";
    displayMessage(message.str());
    return pReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}


LONG WINAPI hookRegOpenKey(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult) {
    wostringstream message;
    message << L"RegOpenKey() was called. Subkey: " << (wstring)lpSubKey << L"\n";
    displayMessage(message.str());
    return pRegOpenKey(hKey, lpSubKey, phkResult);
}

LSTATUS WINAPI hookRegCloseKey(HKEY hKey) {
    wostringstream message;
    message << L"RegCloseKey() was called.\n";
    displayMessage(message.str());
    return pRegCloseKey(hKey);
}

BOOL WINAPI hookDeleteFile(LPCWSTR lpFileName)
{
    wostringstream message;
    message << L"DeleteFile() was called. File name: " << (wstring)lpFileName << L"\n";
    displayMessage(message.str());
    return pDeleteFile(lpFileName);
}

LSTATUS WINAPI hookRegDeleteKey(HKEY hKey, LPCWSTR lpSubKey)
{
    wostringstream message;
    message << L"RegDeleteKey() was called. Subkey: " << (wstring)lpSubKey << L"\n";
    displayMessage(message.str());
    return pRegDeleteKey(hKey, lpSubKey);
}

LSTATUS WINAPI hookRegCreateKey(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult)
{
    wostringstream message;
    message << L"RegCreateKey() was called. Subkey: " << (wstring)lpSubKey << L"\n";
    displayMessage(message.str());
    return pRegCreateKey(hKey, lpSubKey, phkResult);
}

VOID displayMessage(wstring message)
{
    if (hStdOut != NULL) {
        WriteConsole(hStdOut, message.c_str(), message.size(), NULL, NULL);
    }
}
