#pragma once
#include "PhoneBookRecord.h"
#include "List.h"
#define TABLE_SIZE 512


class HashTable {
private:

	//Массив списков
	List* ListsOfNodes[TABLE_SIZE];
	int indexingType;// SURNAME?PHONE?STREET

public:

	HashTable(int i);

	vector<PhoneBookRecord*> Find(std::string value);

	void Add(PhoneBookRecord* value);

	void Delete(PhoneBookRecord* value);

	void Change(PhoneBookRecord* oldValue, PhoneBookRecord* newValue);

	int HashFunc(std::string word);

};