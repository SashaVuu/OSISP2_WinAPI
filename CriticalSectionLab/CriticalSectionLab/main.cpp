#include <iostream>
#include <windows.h>
#include <vector>

using namespace std;

TCHAR szName[] = TEXT("MyFileMappingObject");

CRITICAL_SECTION*  crSection;
LPCWSTR pathToProccess = L"C:\\Users\\shell\\Documents\\GitHub\\OSISP2_WinAPI\\CriticalSectionLab\\x64\\Debug\\Proccess.exe";   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const int AMOUNT_PROCCESSES = 3;

STARTUPINFO startupInformation[AMOUNT_PROCCESSES];
PROCESS_INFORMATION processInformation[AMOUNT_PROCCESSES];


int main()
{

    //�������� ������� "�������� �����" ��� �������������� ��������������
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // ������������� ����� ��������
        NULL,                    // ������ �� ��������� 
        PAGE_READWRITE,          // ������ � ������/������
        0,                       // ����. ������ ������� 
        sizeof(crSection),       // ������ ������  
        szName);                 // ��� ����������� � ������ �������
    if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE)
    {
        printf("Cannot create a memory-reflected object (%d).\n", GetLastError());
        return -1;
    }



    //������� ���������� ��������� �� ������������� ������ �����.
    crSection = (CRITICAL_SECTION*)MapViewOfFile(
        hMapFile,            // ���������� ����������� � ������ �������
        FILE_MAP_ALL_ACCESS, // ���������� ������/������
        0,
        0,
        sizeof(crSection));
    if (crSection == NULL)
    {
        printf("Projected file presentation is not possible (%d).\n", GetLastError());
        return -1;
    }



    InitializeCriticalSection(crSection);



    //�������� ���������
    for (int i = 0;i < AMOUNT_PROCCESSES;i++) 
    {
        ZeroMemory(&processInformation[i],sizeof(PROCESS_INFORMATION));
        if (CreateProcess(pathToProccess, NULL, 0, 0, TRUE, 0, 0, NULL, &startupInformation[i], &processInformation[i]))
        {
            cout << "Created : "<< processInformation[i].dwProcessId << endl;
            Sleep(100);
        };
    }


    //�������� ���������� ���������
    for (int i = 0;i < AMOUNT_PROCCESSES;i++)
    {   
        WaitForSingleObject(processInformation[i].hProcess, INFINITE);
    }


    //������� ��������
    DeleteCriticalSection(crSection);
    UnmapViewOfFile(crSection);
    CloseHandle(hMapFile);

    return 0;

}
