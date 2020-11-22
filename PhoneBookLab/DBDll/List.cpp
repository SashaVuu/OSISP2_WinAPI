#include "pch.h"
#include "List.h"


List::List() {
	head = new Node();
	head->next = NULL;
}


void List::Add(PhoneBookRecord* value, int fieldType)
{
	if (head->next == NULL) {

		Node* tmp = new Node();
		tmp->value = value;
		tmp->next = NULL;

		if (fieldType == BY_PHONENUM) { tmp->searchParam = value->phoneNum; }
		if (fieldType == BY_STREET) { tmp->searchParam = value->street; }
		if (fieldType == BY_SURNAME) { tmp->searchParam = value->surname; }

		head->next = tmp;

	}

	else {
		Node* last = head;
		while (last->next) {
			last = last->next;
		}
		Node* tmp = new Node();
		tmp->value = value;
		tmp->next = NULL;
		last->next = tmp;
	}

}

void List::Delete(PhoneBookRecord* deleteRecord) {
	Node* post = head;
	Node* prev;
	PhoneBookRecord* record;
	while (post->next) {
		prev = post;
		post = post->next;
		record = post->value;
		if (isIdenticalRecords(record, deleteRecord))
		{
			// prev -> post->  
			prev->next = post->next;
			free(post);
			break;
		}

	}
}

PhoneBookRecord* List::Find(string value) {
	Node* temp = head;
	while (temp->next) {
		temp = temp->next;
		if (temp->searchParam == value)
		{
			return temp->value;
		}
	}
	return NULL;
}

vector<PhoneBookRecord*> List::ListToVector()
{
	vector<PhoneBookRecord*> vector;
	Node* temp = head;
	while (temp->next) {
		temp = temp->next;
		vector.push_back(temp->value);
	}
	return vector;
}

void List::Change(PhoneBookRecord* oldRecord, PhoneBookRecord* newRecord) 
{
	Node* temp = head;
	PhoneBookRecord* record;
	while (temp->next) {
		temp = temp->next;
		record = temp->value;
		record;

		if (isIdenticalRecords(record,oldRecord))
		{
			temp->value = newRecord;
		}	
	}
}

bool List::isIdenticalRecords(PhoneBookRecord* record1, PhoneBookRecord* record2)
{
	bool result =
		record1->blockNum == record2->blockNum &&
		record1->name == record2->name &&
		record1->surname == record2->surname &&
		record1->patronymic == record2->patronymic &&
		record1->street == record2->street &&
		record1->houseNum == record2->houseNum &&
		record1->blockNum == record2->blockNum &&
		record1->flatNum == record2->flatNum;
	return result;
}