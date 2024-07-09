// inejexe.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

HHOOK g_hHook = NULL;

LRESULT GetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
    return ::CallNextHookEx(g_hHook, code, wParam, lParam);
}

int main()
{
    // 模块句柄
    HMODULE hDll = NULL;
    hDll = ::LoadLibrary("Dll1.dll");
    //对寄送至消息队列的消息进行监视
    g_hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, hDll, 0);
    if (g_hHook == NULL) {
        std::cout << "Hook Failed!\n";
        system("pause");
        return -1;
    }
    else {
        std::cout << "Hook Success!\n";
        system("pause");
    }
    ::UnhookWindowsHookEx(g_hHook);
    std::cout << "UnHook\n";
    system("pause");
    return 0;
}

