#pragma once
#include "pch.h"
#include "PhoneBookRecord.h"
#include "HashTable.h"
#include <fstream>


#ifdef SOME_EXPORTS
#define DATABASE_API  __declspec(dllimport)
#else
#define DATABASE_API __declspec(dllexport)
#endif

/*PhoneBookDB();
~PhoneBookDB();*/
DATABASE_API void __cdecl CreateDB();
DATABASE_API void __cdecl DestroyDB();

DATABASE_API vector<PhoneBookRecord*> __cdecl GetRecordsByValue(string value, int indexingType);

//Получаем вектор записей(<=28) на след блоках файла
DATABASE_API vector<PhoneBookRecord*> __cdecl GetNext();

//Получаем вектор начальных записей(<=28)
DATABASE_API vector<PhoneBookRecord*> __cdecl ToBegin();

DATABASE_API vector<PhoneBookRecord*> __cdecl Refresh();

//---------------------------------------
int index;
HashTable* surnameHashTable;
HashTable* streetHashTable;
HashTable* phoneHashTable;

LPVOID SetFilePointer(HANDLE hMap, LPVOID prevFilePointer, DWORD offset);

std::vector<string> getFields(string line);

void  CreateHashTablesByVector(int indexingType);

string* getLineFromDataBlock();

vector<PhoneBookRecord*> GetAllRecordsInBlock();
