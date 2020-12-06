#pragma once
#include "PhoneBookRecord.h"
#include "List.h"
#define TABLE_SIZE 512


class HashTable {
private:

	List* ListsOfNodes[TABLE_SIZE]; //Массив списков
	int indexingType;				//SURNAME?PHONE?STREET

public:

	HashTable(int i);

	vector<PhoneBookRecord*> Find(std::string value);

	void Add(PhoneBookRecord* value);

	int HashFunc(std::string word);

	int GetIndex(PhoneBookRecord* value);

};