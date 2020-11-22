#include "pch.h"
#include "PhoneBookDB.h"


//-Считывает данные с файла
//-Создает и заполняет хэш-таблицы 
DATABASE_API void __cdecl CreateDB()
{
    surnameHashTable = new HashTable(BY_SURNAME);
    streetHashTable = new HashTable(BY_STREET);
    phoneHashTable = new HashTable(BY_PHONENUM);
    pathToDB = "..\\jija.txt";
    records = LoadDB(pathToDB);//читаем с файла и формируем вектор записей
    CreateHashTablesByVector();
}


//-Записывает вектор записей в файл
DATABASE_API void __cdecl DestroyDB()
{
    WriteToFile();
}

//Добавление новой записи
DATABASE_API void __cdecl AddRecord(PhoneBookRecord* newRecord)
{
    records.push_back(newRecord);
    surnameHashTable->Add(newRecord);
    phoneHashTable->Add(newRecord);
    streetHashTable->Add(newRecord);

}


//Запись в файл данных из вектора записей
void  WriteToFile()
{

    ofstream fout;
    fout.open(pathToDB);
    if (fout.is_open()) {
        fout.clear();
        string line;
        for (int i = 0;i < records.size();i++)
        {
            line = "";
            line += records[i]->name + "*";
            line += records[i]->surname + "*";
            line += records[i]->patronymic + "*";
            line += records[i]->phoneNum + "*";
            line += records[i]->street + "*";
            line += to_string(records[i]->houseNum) + "*";
            line += to_string(records[i]->blockNum) + "*";
            line += to_string(records[i]->flatNum) + "\n";
            fout << line;
        }
        fout.close();
    }

}

//Получаем вектор строк что-бы заполнить структуру
std::vector<string>  getFields(string line)
{
    vector<string> fields;
    int startPos = 0;
    int endPos = line.find("*");
    while (endPos != -1) {
        fields.push_back(line.substr(startPos, endPos - startPos));
        startPos = endPos + 1;
        endPos = line.find("*", startPos);
    }
    fields.push_back(line.substr(startPos, line.length() - startPos));
    return fields;
}

//Выгружаем данные с файла и формируем вектор
std::vector<PhoneBookRecord*> LoadDB(string path)
{
    ifstream dataFile;
    vector<PhoneBookRecord*> records;
    dataFile.open(path);

    if (dataFile.is_open()) {
        string line;
        while (getline(dataFile, line)) {
            vector<string> fileRecord = getFields(line);
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
        }
        dataFile.close();
    }
    return records;
}

//Создание хэш-таблиц
void  CreateHashTablesByVector()
{
    for (int i = 0;i < records.size();i++)
    {
        surnameHashTable->Add(records[i]); //внутри метода оно само берет нужное поле
        phoneHashTable->Add(records[i]);
        streetHashTable->Add(records[i]);
    }
}


//Возврат всех записей
DATABASE_API std::vector<PhoneBookRecord*> __cdecl GetAllRecords()
{
    return records;
}

//Возврат записей по значению: ФАМИЛИИ или УЛИЦЫ или НОМЕРА
DATABASE_API std::vector<PhoneBookRecord*> __cdecl GetRecordsByValue(string value, int indexingType)
{
    vector<PhoneBookRecord*> result;
    if (indexingType == BY_SURNAME) { result = surnameHashTable->Find(value); }
    if (indexingType == BY_PHONENUM) { result = phoneHashTable->Find(value); }
    if (indexingType == BY_STREET) { result = streetHashTable->Find(value); }
    return result;
}

DATABASE_API void __cdecl  DeleteRecord(PhoneBookRecord* Record)
{
    for (int i = 0;i < records.size();i++) {
        if (records[i]->blockNum == Record->blockNum &&
            records[i]->name == Record->name &&
            records[i]->surname == Record->surname &&
            records[i]->patronymic == Record->patronymic &&
            records[i]->street == Record->street &&
            records[i]->houseNum == Record->houseNum &&
            records[i]->blockNum == Record->blockNum &&
            records[i]->flatNum == Record->flatNum)

        {   records.erase(records.begin() + i); }
    }

    phoneHashTable->Delete(Record);
    surnameHashTable->Delete(Record);
    streetHashTable->Delete(Record);

}

DATABASE_API void __cdecl ChangeRecord(PhoneBookRecord* oldRecord, PhoneBookRecord* newRecord) 
{
    for (int i = 0;i < records.size();i++) {
        if( records[i]->blockNum== oldRecord->blockNum && 
            records[i]->name == oldRecord->name &&
            records[i]->surname == oldRecord->surname &&
            records[i]->patronymic == oldRecord->patronymic &&
            records[i]->street == oldRecord->street &&
            records[i]->houseNum == oldRecord->houseNum &&
            records[i]->blockNum == oldRecord->blockNum &&
            records[i]->flatNum == oldRecord->flatNum)
        {
            records[i]=newRecord;
        }
    }
    phoneHashTable->Change(oldRecord,newRecord);
    surnameHashTable->Change(oldRecord, newRecord);
    streetHashTable->Change(oldRecord, newRecord);

}