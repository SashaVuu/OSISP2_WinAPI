#include <windows.h>
#include "resource1.h"
#include<math.h>

#define ID_BUTTON_LINE 2001
#define ID_BUTTON_PEN 2002
#define ID_BUTTON_POLYLINE 2003
#define ID_BUTTON_RECTANGLE 2004
#define ID_BUTTON_ELLIPSE 2005
#define ID_BUTTON_POLYGON 2006
#define ID_BUTTON_TEXT 2007
#define ID_BUTTON_CLEAR 2008


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CreateProgramMenu(HWND hwnd);


int WINAPI wWinMain (HINSTANCE hInstance,HINSTANCE , PWSTR sxCmdLine, int nCmdShow)
{
	
	MSG msg{};

	HWND hwnd{};

	WNDCLASSEX wcex{ sizeof(WNDCLASSEX) };

	wcex.cbSize = sizeof(WNDCLASSEX);	
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;  
	wcex.cbClsExtra = 0;		
	wcex.cbWndExtra = 0;		
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; 
	wcex.lpszClassName = L"HelloWorldClass";
	wcex.hIconSm = wcex.hIcon;

	if (!RegisterClassExW(&wcex)) 
	{
		return EXIT_FAILURE;
	}

	//Создание окна
	hwnd = CreateWindow(wcex.lpszClassName, L"MiniPaint", WS_OVERLAPPEDWINDOW,
		0, 0, 600, 400, NULL, NULL, wcex.hInstance, NULL);
	if (hwnd == INVALID_HANDLE_VALUE )
	{
		return EXIT_FAILURE;
	}


	ShowWindow(hwnd,nCmdShow);
	UpdateWindow(hwnd);

	//Цикл обработки сообщений
	while (GetMessage(&msg,NULL,0,0)) 
	{
		//расшифровывает системное сообщение
		TranslateMessage(&msg);

		//передает сообщение в оконную процедуру на обработку
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);

}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HFONT hfont;
	RECT clientRect;

	static int choosenState;
	HDC hdc;// дескриптор контекста устройства окна 
	HBRUSH hBrush;
	HPEN hPen;

	HINSTANCE hInst;

	switch (message)
	{

	case WM_CREATE:
	{
		CreateProgramMenu(hWnd);
		break;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}

	case WM_PAINT: 
	{
		
		hdc = BeginPaint(hWnd, &ps);
		// Проверка достоверности рабочей области.
		ValidateRect(hWnd, NULL);
		EndPaint(hWnd, &ps);
		
		break;
	}
	case WM_LBUTTONUP:
	{
		//Получаем контекст для рисования
		hdc = GetDC(hWnd);
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		switch (choosenState)
		{
			case ID_BUTTON_RECTANGLE:
			{
				hBrush = CreateSolidBrush(RGB(255, 0, 67));
				SelectObject(hdc, hBrush); //делаем кисть активной
				Rectangle(hdc, x, y, x + 100, y + 100);
				break;
			}
			case ID_BUTTON_LINE:
			{
				int widthPen = 4;
				hPen = CreatePen(PS_DASH, widthPen, RGB(0, 0, 0)); //Создаётся объект
				SelectObject(hdc, hPen); //Объект делается текущим
				MoveToEx(hdc, x, y, NULL);
				LineTo(hdc, x+100, y+100);
				break;
			}
			case ID_BUTTON_POLYGON:
			{
				POINT poly[4];

				poly[0].x = x;
				poly[0].y = y;

				poly[1].x = x+200;
				poly[1].y = y+100;

				poly[2].x = x+100;
				poly[2].y = y-10;

				poly[3].x = x;
				poly[3].y = y;

				Polyline(hdc, poly, 4);
				break;
			}
			case ID_BUTTON_POLYLINE:
			{
				POINT poly[4];

				poly[0].x = x;
				poly[0].y = y;

				poly[1].x = x+100;
				poly[1].y = y+100;

				poly[2].x = x-100;
				poly[2].y = y-70;

				poly[3].x = x-150;
				poly[3].y = y-100;

				Polyline(hdc, poly, 4);
				break;
			}
			case ID_BUTTON_ELLIPSE:
			{
				hBrush = CreateSolidBrush(RGB(10, 180, 100));
				SelectObject(hdc, hBrush);
				Ellipse(hdc, x, y, x+100, y+100);
				break;
			}
			case ID_BUTTON_TEXT:
			{
				//Текст 
				TextOut(hdc, x, y, L"ку", strlen("ky"));
				break;
			}

		}
		break;
	}
	case WM_COMMAND:
	{
		hdc = GetDC(hWnd);
		//int x = LOWORD(lParam);
		//int y = HIWORD(lParam);

		switch (LOWORD(wParam))
		{
			
			case ID_BUTTON_LINE:
			{
				choosenState = ID_BUTTON_LINE;
				break;
			}
			case ID_BUTTON_RECTANGLE:
			{
				choosenState = ID_BUTTON_RECTANGLE;
				break;
			}
			case ID_BUTTON_POLYGON:
			{
				choosenState = ID_BUTTON_POLYGON;
				break;
			}
			case ID_BUTTON_POLYLINE:
			{
				choosenState = ID_BUTTON_POLYLINE;
				break;
			}
			case ID_BUTTON_ELLIPSE:
			{
				choosenState = ID_BUTTON_ELLIPSE;
				break;
			}
			case ID_BUTTON_TEXT:
			{
				choosenState = ID_BUTTON_TEXT;
				break;
			}
			case ID_BUTTON_CLEAR:
			{
				RECT r;
				GetClientRect(hWnd, &r);
				ReleaseDC(hWnd,hdc);
				InvalidateRect(hWnd, &r,true);
				UpdateWindow(hWnd);
				break;
			}
		}
		break;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CreateProgramMenu(HWND hwnd)
{
	HMENU mainMenu = CreateMenu();
	HMENU figuresPopupMenu = CreatePopupMenu();

	AppendMenu(mainMenu, MF_STRING | MF_POPUP, (UINT)figuresPopupMenu, L"&Фигуры");
	{

		AppendMenu(figuresPopupMenu, MF_STRING, ID_BUTTON_LINE, L"Линия");
		AppendMenu(figuresPopupMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(figuresPopupMenu, MF_STRING, ID_BUTTON_POLYLINE, L"Ломаная");
		AppendMenu(figuresPopupMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(figuresPopupMenu, MF_STRING, ID_BUTTON_RECTANGLE, L"Прямоугольник");
		AppendMenu(figuresPopupMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(figuresPopupMenu, MF_STRING, ID_BUTTON_POLYGON, L"Многоугольник");
		AppendMenu(figuresPopupMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(figuresPopupMenu, MF_STRING, ID_BUTTON_ELLIPSE, L"Эллипс");
		AppendMenu(figuresPopupMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(figuresPopupMenu, MF_STRING , ID_BUTTON_TEXT, L"Текст");
		AppendMenu(figuresPopupMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(figuresPopupMenu, MF_STRING, ID_BUTTON_CLEAR, L"Очистить");
	}

	SetMenu(hwnd, mainMenu);
}
