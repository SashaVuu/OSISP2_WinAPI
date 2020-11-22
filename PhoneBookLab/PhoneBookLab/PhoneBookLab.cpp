// PhoneBookLab.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "PhoneBookLab.h"
#include <string>

#define MAX_LOADSTRING 100
#define EDIT_WIDTH 150
#define EDIT_HEIGHT 20
#define AMOUNT_FIELDS 8
#define COLUMN_WIDTH 90


//BUTTONS
#define WM_FIND_BUTTON 10000
#define WM_DELETE_BUTTON 10001
#define WM_REFRESH_BUTTON 10002
#define WM_CHANGE_BUTTON 10003
#define WM_FILL_CHANGE_BUTTON 10004
#define WM_ADD_BUTTON 10005


#define WM_LISTVIEW 20000
#define WM_COMBOBOX 20001


#define WM_EDIT_1 30001
#define WM_EDIT_2 30002
#define WM_EDIT_3 30003
#define WM_EDIT_4 30004
#define WM_EDIT_5 30005
#define WM_EDIT_6 30006
#define WM_EDIT_7 30007
#define WM_EDIT_8 30008




//LibFunc
#define CREATE_FUNCTION "?CreateDB@@YAXXZ"
#define GET_ALL_REC_FUNCTION "?GetAllRecords@@YA?AV?$vector@PAVPhoneBookRecord@@V?$allocator@PAVPhoneBookRecord@@@std@@@std@@XZ"
#define DESTROY_FUNCTION "?DestroyDB@@YAXXZ"
#define GET_REC_BY_VAL_FUNCTION "?GetRecordsByValue@@YA?AV?$vector@PAVPhoneBookRecord@@V?$allocator@PAVPhoneBookRecord@@@std@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@2@H@Z"
#define ADD_RECORD_FUNCTION "?AddRecord@@YAXPAVPhoneBookRecord@@@Z"
#define DELETE_RECORD_FUNCTION  "?DeleteRecord@@YAXPAVPhoneBookRecord@@@Z"
#define CHANGE_RECORD_FUNCTION  "?ChangeRecord@@YAXPAVPhoneBookRecord@@0@Z"


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

HWND ComboBox;
HWND MainInputEdit;

HWND ChangeInputEdits[AMOUNT_FIELDS];
HWND ListView;
LPWSTR comboboxElem;
PhoneBookRecord* choosedRecord;

//Заголовки функций
VOID CreateColumns(HWND hWndListView);
VOID FillListView(HWND hWnd, std::vector<PhoneBookRecord*> records);
LPWSTR strToLPWSTR(std::string s);
PhoneBookRecord* getRecordFromListView(HWND hWnd, int index);


//Библиотека
HMODULE hmd = LoadLibrary(L"DBDll.dll");

typedef void(*del)();
del CreateDB = (del)GetProcAddress(hmd, CREATE_FUNCTION);

typedef vector<PhoneBookRecord*>(*loadDB)();
loadDB getAllRecords = (loadDB)GetProcAddress(hmd, GET_ALL_REC_FUNCTION);

typedef void(*add)(PhoneBookRecord*);
add AddRecord = (add)GetProcAddress(hmd, ADD_RECORD_FUNCTION);

typedef vector<PhoneBookRecord*>(*getRecDB)(string, int);
getRecDB getRecordByValue = (getRecDB)GetProcAddress(hmd, GET_REC_BY_VAL_FUNCTION);

typedef void(*deletee)(PhoneBookRecord*);
deletee DeleteRecord = (deletee)GetProcAddress(hmd, DELETE_RECORD_FUNCTION);

typedef void(*chng)(PhoneBookRecord*, PhoneBookRecord*);
chng ChangeRecord = (chng)GetProcAddress(hmd, CHANGE_RECORD_FUNCTION);


typedef void(*destroy)();
destroy DestroyDB = (destroy)GetProcAddress(hmd, DESTROY_FUNCTION);



const std::vector<LPCWSTR> colNames{
    L"Phone", 
    L"Name", 
    L"Surname",
    L"Patronymic",
    L"Street", 
    L"House",
    L"Block",
    L"Flat" };


// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PHONEBOOKLAB, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PHONEBOOKLAB));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PHONEBOOKLAB));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PHONEBOOKLAB);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   //Основное окно
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }


   //Таблица с выводом
   INITCOMMONCONTROLSEX icex ;
   RECT rcClient;
   icex.dwICC = ICC_LISTVIEW_CLASSES;
   InitCommonControlsEx(&icex);
   GetClientRect(hWnd, &rcClient);

   ListView = CreateWindowEx(
       NULL,
       WC_LISTVIEW,
       L"",
       WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
       0,
       0,
       COLUMN_WIDTH*AMOUNT_FIELDS,
       190,
       hWnd,
       (HMENU)WM_LISTVIEW,
       GetModuleHandle(NULL),
       NULL);

   CreateColumns(ListView);
   ShowWindow(ListView, SW_SHOWDEFAULT);

   //Комбобокс с выбором критерия поиска (Фамилия, улица, номер)
   ComboBox = CreateWindow(
       WC_COMBOBOX, 
       TEXT(""),
       CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
       850,
       20,
       EDIT_WIDTH,
       200,
       hWnd,
       (HMENU)WM_COMBOBOX,
       GetModuleHandle(NULL),
       NULL);
   //Добавление строк в комбобокс
   SendMessage(ComboBox, CB_ADDSTRING, 0, (LPARAM)L"Surname");
   SendMessage(ComboBox, CB_ADDSTRING, 0, (LPARAM)L"Phone");
   SendMessage(ComboBox, CB_ADDSTRING, 0, (LPARAM)L"Street");


   //ShowWindow(hWndComboBox, SW_SHOWDEFAULT);

   //Поле для ввода Фамилии,улицы,номера
   MainInputEdit = CreateWindow(
       L"Edit",
       NULL,
       WS_BORDER | WS_CHILD | WS_VISIBLE | NULL | NULL,
       850,
       60,
       EDIT_WIDTH,
       EDIT_HEIGHT,
       hWnd,
       NULL,
       NULL,
       0);

   //Поля для ввода, при изменении записи
   for (int i = 0;i < AMOUNT_FIELDS;i++) {
       ChangeInputEdits[i] = CreateWindow(
           L"Edit",
           NULL,
           WS_BORDER | WS_CHILD | WS_VISIBLE | NULL | NULL,
           850,
           180 + 40 * i,
           EDIT_WIDTH,
           EDIT_HEIGHT,
           hWnd,
           (HMENU)WM_EDIT_1+i,
           NULL,
           0);  
   }


   LPCWSTR buttonNames[] = { L"Find" ,L"Delete" ,L"Refresh", L"Change"};

   for (int i = 0; i < 4; i++) {
       HWND Buttons = CreateWindow(
           L"BUTTON",
           buttonNames[i],
           WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
           750 + 70 * i,
           100,
           60,
           20,
           hWnd,
           (HMENU)(WM_FIND_BUTTON + i),
           (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
           NULL);
   }

   //Кнопка ОК- которая изменяет запись после заполнения всех полей
   HWND OkButton = CreateWindow(
       L"BUTTON",
       L"OK",
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       750+190,
       500,
       60,
       20,
       hWnd,
       (HMENU)(WM_FILL_CHANGE_BUTTON),
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL
   );

   //Кнопка ADD - которая добавляет новую запись
   HWND AddButton = CreateWindow(
       L"BUTTON",
       L"Add",
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
       750,
       500,
       60,
       20,
       hWnd,
       (HMENU)(WM_ADD_BUTTON),
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL);

   //Заполняем таблицу
   CreateDB();
   vector<PhoneBookRecord*> records = getAllRecords();
   FillListView(ListView, records);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {

            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {

            case WM_FIND_BUTTON:
            {

                //Считывание и преобразование текста с поля для ввода
                LPWSTR lpwstr = strToLPWSTR("");
                GetWindowText(MainInputEdit, lpwstr, -1);

                char editBuff[50];
                wcstombs(editBuff, lpwstr, 50);
                std::string editStr = std::string(editBuff);
                if (editStr.size() == 0)
                    break;

                std::vector<PhoneBookRecord*> records;

                //Считывание текста с комбобокса
                comboboxElem = strToLPWSTR("");
                GetWindowText(ComboBox, comboboxElem, 15);

                if (lstrcmpW((LPCWSTR)comboboxElem, L"Phone") == 0) {records = getRecordByValue(editStr, BY_PHONENUM);}
                if (lstrcmpW((LPCWSTR)comboboxElem, L"Street") == 0) {records = getRecordByValue(editStr, BY_STREET);}
                if (lstrcmpW((LPCWSTR)comboboxElem, L"Surname") == 0) {records = getRecordByValue(editStr, BY_SURNAME);}

                FillListView(ListView, records);
                break;
            }

            case WM_ADD_BUTTON:
            {
                vector<string> fields;
                bool isParametersFilled=true;
                for (int i = 0; i < 8; i++) {

                    /* Get the window text */
                    TCHAR* buf;
                    int len;
                    buf = (TCHAR*)malloc(len = ((GetWindowTextLength(ChangeInputEdits[i]) + 1) * sizeof(TCHAR)));

                    if (buf != NULL)
                    {
                        GetWindowText(ChangeInputEdits[i], buf, len);
                    }

                    char buffer[500];
                    wcstombs(buffer, buf, 255);

                    free(buf);
                    string field = string(buffer);

                    if (field.size() == 0) {
                        MessageBox(hWnd, L"Please enter all parameters", L"Error", MB_OK | MB_ICONINFORMATION);
                        isParametersFilled = false;
                        break;
                    }

                    fields.push_back(field);

                }

                if (isParametersFilled) {

                    PhoneBookRecord* record = new PhoneBookRecord();
                    record->phoneNum = fields[0];
                    record->name = fields[1];
                    record->surname = fields[2];
                    record->patronymic = fields[3];
                    record->street = fields[4];
                    record->houseNum = stoi(fields[5]);
                    record->blockNum = stoi(fields[6]);
                    record->flatNum = stoi(fields[7]);

                    AddRecord(record);

                    vector<PhoneBookRecord*> records = getAllRecords();
                    FillListView(ListView, records);

                }
                break;
            }
            case WM_DELETE_BUTTON:
            {
                //***
                int index = ListView_GetNextItem(ListView, -1, LVNI_SELECTED);
                if (index == -1) {
                    MessageBox(hWnd, L"Choose record in list view", L"Error", MB_OK | MB_ICONINFORMATION);
                    break;
                }

                choosedRecord = getRecordFromListView(ListView, index);
                
                DeleteRecord(choosedRecord);
                vector<PhoneBookRecord*> records = getAllRecords();
                FillListView(ListView, records);
                choosedRecord = NULL;

                break;
            }
            case WM_CHANGE_BUTTON:
            {
                //***Заполняем edits полями записи,вытащенной из таблицы
                int index = ListView_GetNextItem(ListView, -1, LVNI_SELECTED);
                if (index == -1) {
                    MessageBox(hWnd, L"Choose record in list view", L"Error", MB_OK | MB_ICONINFORMATION);
                    break;
                }

                choosedRecord = getRecordFromListView(ListView,index);

                SetWindowText(ChangeInputEdits[0], wstring(choosedRecord->phoneNum.begin(), choosedRecord->phoneNum.end()).c_str());
                SetWindowText(ChangeInputEdits[1], wstring(choosedRecord->name.begin(), choosedRecord->name.end()).c_str());
                SetWindowText(ChangeInputEdits[2], wstring(choosedRecord->surname.begin(), choosedRecord->surname.end()).c_str());
                SetWindowText(ChangeInputEdits[3], wstring(choosedRecord->patronymic.begin(), choosedRecord->patronymic.end()).c_str());
                SetWindowText(ChangeInputEdits[4], wstring(choosedRecord->street.begin(), choosedRecord->street.end()).c_str());
                SetWindowText(ChangeInputEdits[5], to_wstring(choosedRecord->houseNum).c_str());
                SetWindowText(ChangeInputEdits[6], to_wstring(choosedRecord->blockNum).c_str());
                SetWindowText(ChangeInputEdits[7], to_wstring(choosedRecord->flatNum).c_str());

                break;
            }

            case WM_FILL_CHANGE_BUTTON:
            {
                //***После того как (не)изменили строки в edits, создаем запись и меняем ее с изначальной

                bool isParametersFilled = true;
                vector<string> fields;

                for (int i=0; i < 8; i++) {

                    /* Get the window text */
                    TCHAR* buf;
                    int len;
                    buf = (TCHAR*)malloc(len = ((GetWindowTextLength(ChangeInputEdits[i]) + 1) * sizeof(TCHAR)));

                    if (buf != NULL)
                    {
                        GetWindowText(ChangeInputEdits[i], buf, len);
                    }

                    char buffer[500];
                    wcstombs(buffer, buf, 255);

                    free(buf);
                    string field = string(buffer);

                    if (field.size() == 0) {
                        MessageBox(hWnd, L"Please enter all parameters", L"Error", MB_OK | MB_ICONINFORMATION);
                        isParametersFilled = false;
                        break;
                    }

                    if (choosedRecord==NULL) {
                        MessageBox(hWnd, L"Please choose record to change", L"Error", MB_OK | MB_ICONINFORMATION);
                        isParametersFilled = false;
                        break;
                    }

                    fields.push_back(field);

                }

                if (isParametersFilled) {

                    PhoneBookRecord* record = new PhoneBookRecord();
                    record->phoneNum = fields[0];
                    record->name= fields[1];
                    record->surname = fields[2];
                    record->patronymic = fields[3];
                    record->street = fields[4];
                    record->houseNum = stoi(fields[5]);
                    record->blockNum = stoi(fields[6]);
                    record->flatNum = stoi(fields[7]);

                    ChangeRecord(choosedRecord,record);

                    vector<PhoneBookRecord*> records = getAllRecords();
                    FillListView(ListView,records);
                    choosedRecord = NULL;
                }
                break;
            }
            case WM_REFRESH_BUTTON:
            {
                //***Выводим все записи в таблицу
                vector<PhoneBookRecord*> records = getAllRecords();
                FillListView(ListView, records);
                break;

            }
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    //Отрисовываем все подписи
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);


            //Строки
            RECT rec;
            GetClientRect(hWnd, &rec);
            rec.left += 750;
            rec.top += 20;

            DrawText(hdc, L"Choose:", -1, &rec, DT_TOP | DT_LEFT);
            rec.top += 40;
            DrawText(hdc, L"Enter:", -1, &rec, DT_TOP | DT_LEFT);
            rec.top += 80;

            //Строки - подписи полей
            for (int i=0;i<colNames.size();i++) 
            {
                rec.top += 40;
                DrawText(hdc, colNames[i], -1 , &rec, DT_TOP | DT_LEFT);
            }
            


            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}





//Создаем колонки
VOID CreateColumns(HWND hWndListView)
{
    RECT rcListView;
    LVCOLUMN lvc;
    GetClientRect(hWndListView, &rcListView);

    lvc.mask = LVCF_WIDTH | LVCF_TEXT;
    lvc.cx = COLUMN_WIDTH;//ширина

    for (int i = 0; i < AMOUNT_FIELDS; i++) {
        lvc.pszText = const_cast<LPWSTR>(colNames[i]);
        ListView_InsertColumn(hWndListView, i, &lvc);
    }
}

//Добавляем записи в таблицу
VOID FillListView(HWND hWnd, std::vector<PhoneBookRecord*> records) 
{
    
    ListView_DeleteAllItems(hWnd);
    LVITEM lvI;

    lvI.pszText = LPSTR_TEXTCALLBACK;
    lvI.mask = LVIF_TEXT;
    lvI.iSubItem = 0;

    for (int i = 0; i < records.size(); i++) {
        lvI.iItem = i;

        ListView_InsertItem(hWnd, &lvI);

        ListView_SetItemText(hWnd, i, 0, strToLPWSTR(records[i]->phoneNum));
        ListView_SetItemText(hWnd, i, 1, strToLPWSTR(records[i]->name));
        ListView_SetItemText(hWnd, i, 2, strToLPWSTR(records[i]->surname));
        ListView_SetItemText(hWnd, i, 3, strToLPWSTR(records[i]->patronymic));
        ListView_SetItemText(hWnd, i, 4, strToLPWSTR(records[i]->street));
        ListView_SetItemText(hWnd, i, 5, strToLPWSTR(std::to_string(records[i]->houseNum)));
        ListView_SetItemText(hWnd, i, 6, strToLPWSTR(std::to_string(records[i]->blockNum)));
        ListView_SetItemText(hWnd, i, 7, strToLPWSTR(std::to_string(records[i]->flatNum)));
    }
}

LPWSTR strToLPWSTR(std::string s)
{
    const char* str = s.c_str();

    wchar_t wtext[255];
    std::mbstowcs(wtext, str, s.size() + 1);
    LPWSTR ptr = wtext;

    return ptr;
}

PhoneBookRecord* getRecordFromListView(HWND hWnd,int index) {

    LPWSTR lpwstr = strToLPWSTR("");
    PhoneBookRecord* record = new PhoneBookRecord();
    vector <string> fields;

    for (int i = 0; i < AMOUNT_FIELDS; i++) {
        ListView_GetItemText(hWnd, index, i, lpwstr, 256);
        char buffer[256];
        int ret;
        ret = wcstombs(buffer, lpwstr, sizeof(buffer));
        std::string s = string(buffer);
        fields.push_back(s);
    }

    record->phoneNum = fields[0];
    record->name = fields[1];
    record->surname = fields[2];
    record->patronymic = fields[3];
    record->street = fields[4];
    record->houseNum = stoi(fields[5]);
    record->blockNum = stoi(fields[6]);
    record->flatNum = stoi(fields[7]);

    return record;
}