#pragma once
#include <string>
using namespace std;


#define BY_SURNAME 0
#define BY_PHONENUM 1
#define BY_STREET 2

//�������, �������, ���, ��������, �����, ���, ������, ��������
class PhoneBookRecord {
public:
	string phoneNum;
	string name;
	string surname;
	string patronymic; //��������
	string street;
	int houseNum;
	int blockNum;
	int flatNum;

};
