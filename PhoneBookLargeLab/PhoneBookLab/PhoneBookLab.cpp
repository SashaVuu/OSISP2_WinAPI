// PhoneBookLab.cpp : Определяет точку входа для приложения.

#include "framework.h"
#include "PhoneBookLab.h"
#include <string>

#define MAX_LOADSTRING 100
#define EDIT_WIDTH 150
#define EDIT_HEIGHT 20
#define AMOUNT_FIELDS 8
#define COLUMN_WIDTH 90


//BUTTONS,COMBOBOX,LISTVIEW,EDITS

#define WM_FIND_BUTTON 10000
#define WM_REFRESH_BUTTON 10001
#define WM_TOSTART_BUTTON 10002
#define WM_NEXT_BUTTON 10003


#define WM_LISTVIEW 20000
#define WM_COMBOBOX 20001



//LibFunc
#define CREATE_FUNCTION "?CreateDB@@YAXXZ"
#define DESTROY_FUNCTION "?DestroyDB@@YAXXZ"
#define GET_NEXT_FUNCTION  "?GetNext@@YA?AV?$vector@PAVPhoneBookRecord@@V?$allocator@PAVPhoneBookRecord@@@std@@@std@@XZ"
#define TO_BEGIN_FUNCTION  "?ToBegin@@YA?AV?$vector@PAVPhoneBookRecord@@V?$allocator@PAVPhoneBookRecord@@@std@@@std@@XZ"
#define FIND_FUNCTION "?GetRecordsByValue@@YA?AV?$vector@PAVPhoneBookRecord@@V?$allocator@PAVPhoneBookRecord@@@std@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@2@H@Z"
#define REFRESH_FUNCTION "?Refresh@@YA?AV?$vector@PAVPhoneBookRecord@@V?$allocator@PAVPhoneBookRecord@@@std@@@std@@XZ"

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

typedef vector<PhoneBookRecord*>(*getRecDB)(string, int);
getRecDB getRecordByValue = (getRecDB)GetProcAddress(hmd, FIND_FUNCTION);

typedef void(*destroy)();
destroy DestroyDB = (destroy)GetProcAddress(hmd, DESTROY_FUNCTION);

typedef vector<PhoneBookRecord*>(*getRecords)();
getRecords GetNext = (getRecords)GetProcAddress(hmd, GET_NEXT_FUNCTION);
getRecords ToBegin = (getRecords)GetProcAddress(hmd, TO_BEGIN_FUNCTION);
getRecords Refresh = (getRecords)GetProcAddress(hmd, REFRESH_FUNCTION);


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
       COLUMN_WIDTH*AMOUNT_FIELDS + 20 ,
       470,
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


   LPCWSTR buttonNames[] = { L"Find" ,L"Refresh"};

   for (int i = 0; i < 2; i++) {
       HWND Buttons = CreateWindow(
           L"BUTTON",
           buttonNames[i],
           WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
           750 + 190 * i,
           100,
           60,
           20,
           hWnd,
           (HMENU)(WM_FIND_BUTTON + i),
           (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
           NULL);
   }

       HWND NextButton = CreateWindow(
           L"BUTTON",
           L"To Begin",
           WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
           10 ,
           500,
           80,
           20,
           hWnd,
           (HMENU)(WM_TOSTART_BUTTON),
           (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
           NULL);

       HWND PrevButton = CreateWindow(
           L"BUTTON",
           L"Next",
           WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
           670,
           500,
           60,
           20,
           hWnd,
           (HMENU)(WM_NEXT_BUTTON),
           (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
           NULL);
   



   //Заполняем таблицу
   CreateDB();
   vector<PhoneBookRecord*> records = GetNext();
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

                TCHAR* buf;
                int len;
                buf = (TCHAR*)malloc(len = ((GetWindowTextLength(MainInputEdit) + 1) * sizeof(TCHAR)));

                if (buf != NULL)
                {
                    GetWindowText(MainInputEdit, buf, len);
                }

                char buffer[500];
                wcstombs(buffer, buf, 255);


                //Строка с edit
                string editStr = string(buffer);

                free(buf);

                if (editStr.size() == 0)
                    break;

                std::vector<PhoneBookRecord*> records;

                //Считывание текста с комбобокса
                comboboxElem = strToLPWSTR("");
                GetWindowText(ComboBox, comboboxElem, 15);

                if (lstrcmpW((LPCWSTR)comboboxElem, L"Phone") == 0) { records = getRecordByValue(editStr, BY_PHONENUM); }
                if (lstrcmpW((LPCWSTR)comboboxElem, L"Street") == 0) { records = getRecordByValue(editStr, BY_STREET); }
                if (lstrcmpW((LPCWSTR)comboboxElem, L"Surname") == 0) { records = getRecordByValue(editStr, BY_SURNAME); }

                FillListView(ListView, records);

                break;
            }

            case WM_NEXT_BUTTON:
            {
                //Выводим все записи в таблицу
                vector<PhoneBookRecord*> records = GetNext();
                FillListView(ListView, records);
                break;

            }
            case WM_TOSTART_BUTTON:
            {
                //Выводим все записи в таблицу
                vector<PhoneBookRecord*> records = ToBegin();
                FillListView(ListView, records);
                break;

            }
            case WM_REFRESH_BUTTON:
            {
                //Выводим все записи в таблицу
                vector<PhoneBookRecord*> records = Refresh();
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

            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
    {
        DestroyDB();
        PostQuitMessage(0);
        break;
    }
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


//------------------------------------------------------------------


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