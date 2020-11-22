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
DATABASE_API void __cdecl AddRecord(PhoneBookRecord* newRecord);
DATABASE_API void __cdecl  DeleteRecord(PhoneBookRecord* deleteRecord);
DATABASE_API void __cdecl ChangeRecord(PhoneBookRecord* oldRecord, PhoneBookRecord* newRecord);
DATABASE_API vector<PhoneBookRecord*> __cdecl GetRecordsByValue(string value, int indexingType);
DATABASE_API vector<PhoneBookRecord*> __cdecl GetAllRecords();


//---------------------------------------
vector<PhoneBookRecord*> records;
HashTable* surnameHashTable;
HashTable* streetHashTable;
HashTable* phoneHashTable;
std::vector<string> getFields(string line);
std::vector<PhoneBookRecord*> LoadDB(string path);
void CreateHashTablesByVector();
void  WriteToFile();
string pathToDB;
