#include <iostream>
#include <string>
#include <functional>
#include "ThreadPool.h"

void beep() {
    cout << "beep" << endl;
}

void peep() {
    Sleep(3000);
}

int main()
{
    ThreadPool* tp = new ThreadPool(3);
    
    for (int i = 0;i <6;i++)
    {
        tp->AddTask(beep);
    }
        
    // ��� �� ���������(����� 2) ��������� ����� 
    // E��� ��� ������ ������, ���� ���� ���-�� ������������
   /* for (int i=0;i<6;i++) 
    {
        tp->AddTask(peep);
    }*/

    delete tp;
    return 0;
}
