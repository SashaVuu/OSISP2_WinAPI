#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

#define REG_PATH L"Software\\TestApplication"


/*FileSystem*/

//1.WriteFile
//2.SetFilePointer
//3.ReadFile
//4.DeleteFile


/*Registry*/

//5.RegOpenKey
//6.RegCloseKey
//7.RegCreateKey
//8.RegDeleteKey


int main()
{

    HANDLE hFile = CreateFile(L"jija.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    string text = "some text";

    Sleep(100);

    //1.
    WriteFile(hFile, text.c_str(), text.size(), NULL, NULL);

    Sleep(100);

    //2.
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

    Sleep(100);

    char buf[50];
    DWORD bufSize = sizeof(buf);
    ZeroMemory(&buf, sizeof(buf));

    //3.
    ReadFile(hFile, buf, text.size(), NULL, NULL);

    CloseHandle(hFile);

    Sleep(100);


    //4.
    DeleteFile(L"jija.txt");

    Sleep(10);
    //------------------------------------------------------------------------------
   
    //5.
    HKEY result;
    RegOpenKey(HKEY_CURRENT_USER, L"Software\\Adobe", &result);

    //6.
    RegCloseKey(result);

    //7.
    HKEY createResult;
    RegCreateKey(HKEY_CURRENT_USER, REG_PATH, &createResult);

    RegCloseKey(createResult);

    //8.
    RegDeleteKey(HKEY_CURRENT_USER, REG_PATH);

    cout << "Done\n";
    return 0;
}

