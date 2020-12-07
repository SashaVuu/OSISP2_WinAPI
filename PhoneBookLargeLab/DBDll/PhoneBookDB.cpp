#include "pch.h"
#include "PhoneBookDB.h"

SYSTEM_INFO sysinfo = { 0 };

HANDLE hFile;
DWORD fsize;
HANDLE hMapping;
LPVOID fPointer;


#define BLOCK_SIZE 65536
#define NEXT_SIZE 28


//Указатель на начало данных
unsigned char* pointer;

//Отступ по блокам
DWORD offset;

//Отступ при хождении по данным
int offsetInDataBlock;

//Записи считанные на данный момент
vector<PhoneBookRecord*> records;
int vectorPos;



DATABASE_API void __cdecl CreateDB()
{
    GetSystemInfo(&sysinfo);
    hFile = CreateFile(L"../jija.txt", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    fsize = GetFileSize(hFile, NULL);
    hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    fPointer = SetFilePointer(hMapping, NULL, BLOCK_SIZE * 0);

    offset = 0;
    pointer = (unsigned char*)fPointer;
    offsetInDataBlock = 0;
    vectorPos = 0;
    records = GetAllRecordsInBlock();

}



DATABASE_API void __cdecl DestroyDB()
{
    UnmapViewOfFile(hFile);
    CloseHandle(hMapping);
    CloseHandle(hFile);
}


//Возврат следующих N записей
DATABASE_API std::vector<PhoneBookRecord*> __cdecl GetNext()
{
    vector<PhoneBookRecord*> result;
    int j = 1;
    while ((j <= NEXT_SIZE) && (vectorPos < records.size()))
    {
        result.push_back(records[vectorPos]);
        j++;
        vectorPos++;
    }
    return result;
}


//Откат в начало
DATABASE_API vector<PhoneBookRecord*> __cdecl ToBegin() 
{
    offsetInDataBlock = 0;
    vectorPos = 0;
    vector<PhoneBookRecord*> result = GetNext();
    return result;

}

/*
* Считываем поблочно данные с файла
* Для каждого считанного блока создается вектор записей, и хэш таблица указанного пользователем типа
* из таблицы вытягиваются нужные записи и формируется результирующий вектор
* После каждой итерации память под таблицей очищается
*/

DATABASE_API std::vector<PhoneBookRecord*> __cdecl GetRecordsByValue(string value, int indexingType)
{
    vector<PhoneBookRecord*> result;
    vector<PhoneBookRecord*> sumResult;
    offset = 0;
    fPointer = SetFilePointer(hMapping, NULL, BLOCK_SIZE * offset);

    offsetInDataBlock = 0;
    vectorPos = 0;

    while (fPointer != NULL)
    {
       
        records = GetAllRecordsInBlock();
        offset++;

        if (indexingType == BY_SURNAME) {
            surnameHashTable = new HashTable(BY_SURNAME);
            CreateHashTablesByVector(BY_SURNAME);
            result = surnameHashTable->Find(value);
            delete(surnameHashTable);

        }
        if (indexingType == BY_PHONENUM) {
            phoneHashTable = new HashTable(BY_PHONENUM);
            CreateHashTablesByVector(BY_PHONENUM);
            result = phoneHashTable->Find(value);
            delete(phoneHashTable);
        }
        if (indexingType == BY_STREET) {
            streetHashTable = new HashTable(BY_STREET);
            CreateHashTablesByVector(BY_STREET);
            result = streetHashTable->Find(value);
            delete(streetHashTable);

        }
        sumResult.insert(sumResult.end(), result.begin(), result.end());

        offsetInDataBlock = 0;
        vectorPos = 0;

        fPointer = SetFilePointer(hMapping, NULL, BLOCK_SIZE * offset);

    }

    offset = 0;

    records = sumResult;

    return GetNext();

}


DATABASE_API vector<PhoneBookRecord*> __cdecl Refresh() 
{
    fPointer = SetFilePointer(hMapping, NULL, BLOCK_SIZE * 0);
    pointer = (unsigned char*)fPointer;
    offsetInDataBlock = 0;
    vectorPos = 0;
    records = GetAllRecordsInBlock();
    vector<PhoneBookRecord*> result = GetNext();
    return result;
}



LPVOID SetFilePointer(HANDLE hMap, LPVOID prevFilePointer, DWORD offset)
{

    LPVOID result;
    if (prevFilePointer != NULL)
        UnmapViewOfFile(prevFilePointer);

    result = MapViewOfFile(hMap, FILE_MAP_READ, 0, offset, sysinfo.dwAllocationGranularity);
    if (result == NULL)
        result = MapViewOfFile(hMap, FILE_MAP_READ, 0, offset, 0);
    return result;

}



//Получаем вектор строк что-бы заполнить структуру
vector<string> getFields(string line)
{
    vector<string> fields;
    int startPos = 0;
    int endPos = line.find(" ");
    while (endPos != -1) {
        fields.push_back(line.substr(startPos, endPos - startPos));
        startPos = endPos + 1;
        endPos = line.find(" ", startPos);
    }
    fields.push_back(line.substr(startPos, line.length() - startPos));
    return fields;
}


string* getLineFromDataBlock()
{
    int i = 0;
    char* lineChar = new char[100];

    while (pointer[offsetInDataBlock] != '\n' && offsetInDataBlock < BLOCK_SIZE-1)
    {
        lineChar[i] = pointer[offsetInDataBlock];
        i++;
        offsetInDataBlock++;
    }

    string* str = NULL;
    if (lineChar[i - 2] == ';') {
        lineChar[i - 2] = '\0';
        str = new string(lineChar);
        offsetInDataBlock++;
    }
    return str;
}


vector<PhoneBookRecord*> GetAllRecordsInBlock()
{
    vector<PhoneBookRecord*> records;
    int pos_begin;
    int pos_end;
    int length;

    string* line = getLineFromDataBlock();

    while (line != NULL) {

        vector<string> fileRecord = getFields(*line);

        PhoneBookRecord* record = new PhoneBookRecord();
        record->name = fileRecord[0];
        record->surname = fileRecord[1];
        record->patronymic = fileRecord[2];
        record->phoneNum = fileRecord[3];
        record->street = fileRecord[4];
        record->houseNum = stoi(fileRecord[5]);
        record->blockNum = stoi(fileRecord[6]);
        record->flatNum = stoi(fileRecord[7]);

        records.push_back(record);

        line = getLineFromDataBlock();

    }

    return records;

};


//Создание хэш-таблиц
void  CreateHashTablesByVector(int indexingType)
{
    if (indexingType == BY_SURNAME) {
        for (int i = 0;i < records.size();i++)
        {
            surnameHashTable->Add(records[i]); 
        }

    }
    if (indexingType == BY_PHONENUM) {
        for (int i = 0;i < records.size();i++)
        {
            phoneHashTable->Add(records[i]);
        }
    }
    if (indexingType == BY_STREET) {
        for (int i = 0;i < records.size();i++)
        {
            streetHashTable->Add(records[i]);
        }

    }
}



