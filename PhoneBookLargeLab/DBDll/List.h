#pragma once
#include <xlocinfo>
#include "PhoneBookRecord.h"
#include <vector>
class Node {
public:
	Node* next;
	PhoneBookRecord* value;
	string searchParam;
	Node()
	{
		next = NULL;
		value = NULL;
		searchParam = "";
	}
};


class List {
public:
	List();
	void Add(PhoneBookRecord* value, int index_by_what);
	PhoneBookRecord* Find(string value);
	vector<PhoneBookRecord*> ListToVector();
	bool isIdenticalRecords(PhoneBookRecord* record1, PhoneBookRecord* record2);
private:
	Node* head;
};
