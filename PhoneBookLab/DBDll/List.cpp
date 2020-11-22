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
		if (record->blockNum == deleteRecord->blockNum &&
			record->name == deleteRecord->name &&
			record->surname == deleteRecord->surname &&
			record->patronymic == deleteRecord->patronymic &&
			record->street == deleteRecord->street &&
			record->houseNum == deleteRecord->houseNum &&
			record->blockNum == deleteRecord->blockNum &&
			record->flatNum == deleteRecord->flatNum)
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

		if (record->blockNum == oldRecord->blockNum &&
			record->name == oldRecord->name &&
			record->surname == oldRecord->surname &&
			record->patronymic == oldRecord->patronymic &&
			record->street == oldRecord->street &&
			record->houseNum == oldRecord->houseNum &&
			record->blockNum == oldRecord->blockNum &&
			record->flatNum == oldRecord->flatNum)
		{
			temp->value = newRecord;
		}	
	}
}
