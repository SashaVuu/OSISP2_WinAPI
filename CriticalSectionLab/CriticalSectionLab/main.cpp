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

    //Создание объекта "проекция файла" для межпроцессного взаимодействия
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // использование файла подкачки
        NULL,                    // защита по умолчанию 
        PAGE_READWRITE,          // доступ к чтению/записи
        0,                       // макс. размер объекта 
        sizeof(crSection),       // размер буфера  
        szName);                 // имя отраженного в памяти объекта
    if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE)
    {
        printf("Cannot create a memory-reflected object (%d).\n", GetLastError());
        return -1;
    }



    //Функция возвращает указатель на представление данных файла.
    crSection = (CRITICAL_SECTION*)MapViewOfFile(
        hMapFile,            // дескриптор отраженного в памяти объекта
        FILE_MAP_ALL_ACCESS, // разрешение чтения/записи
        0,
        0,
        sizeof(crSection));
    if (crSection == NULL)
    {
        printf("Projected file presentation is not possible (%d).\n", GetLastError());
        return -1;
    }



    InitializeCriticalSection(crSection);



    //Создание процессов
    for (int i = 0;i < AMOUNT_PROCCESSES;i++) 
    {
        ZeroMemory(&processInformation[i],sizeof(PROCESS_INFORMATION));
        if (CreateProcess(pathToProccess, NULL, 0, 0, TRUE, 0, 0, NULL, &startupInformation[i], &processInformation[i]))
        {
            cout << "Created : "<< processInformation[i].dwProcessId << endl;
            Sleep(100);
        };
    }


    //Ожидание завершение процессов
    for (int i = 0;i < AMOUNT_PROCCESSES;i++)
    {   
        WaitForSingleObject(processInformation[i].hProcess, INFINITE);
    }


    //Очистка ресурсов
    DeleteCriticalSection(crSection);
    UnmapViewOfFile(crSection);
    CloseHandle(hMapFile);

    return 0;

}
