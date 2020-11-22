#include "pch.h"
#include "HashTable.h"
#include <vector>

HashTable::HashTable(int i)
{
	indexingType = i;
	for (int i = 0;i < TABLE_SIZE;i++)
	{
		ListsOfNodes[i] = new List();
	}
}

std::vector<PhoneBookRecord*> HashTable::Find(std::string value)
{
	int index = HashFunc(value);
	return  ListsOfNodes[index]->ListToVector();

}

void HashTable::Add(PhoneBookRecord* value)
{
	int index = 0;//HashFunc();
	if (indexingType == BY_PHONENUM) { index = HashFunc(value->phoneNum); }
	if (indexingType == BY_STREET) { index = HashFunc(value->street); }
	if (indexingType == BY_SURNAME) { index = HashFunc(value->surname); }
	ListsOfNodes[index]->Add(value, indexingType);
}

void HashTable::Delete(PhoneBookRecord* value)
{
	int index = 0;//HashFunc();
	if (indexingType == BY_PHONENUM) { index = HashFunc(value->phoneNum); }
	if (indexingType == BY_STREET) { index = HashFunc(value->street); }
	if (indexingType == BY_SURNAME) { index = HashFunc(value->surname); }
	ListsOfNodes[index]->Delete(value);
}

void HashTable::Change(PhoneBookRecord* oldValue, PhoneBookRecord* newValue) 
{
	int index = 0;//HashFunc();
	if (indexingType == BY_PHONENUM) { index = HashFunc(oldValue->phoneNum); }
	if (indexingType == BY_STREET) { index = HashFunc(oldValue->street); }
	if (indexingType == BY_SURNAME) { index = HashFunc(oldValue->surname); }
	ListsOfNodes[index]->Change(oldValue, newValue);

}

int HashTable::HashFunc(std::string word)
{
	int sum = 0;
	for (int k = 0; k < word.length(); k++)
		sum = sum + int(word[k]);
	return  sum % TABLE_SIZE;
}