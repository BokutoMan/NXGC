#include <windows.h>
#include <iostream>

int main() {
    DWORD processID;
    std::cout << "Enter Process ID: ";
    std::cin >> processID;

    const char* dllPath = "./MyDLL.dll"; // 替换为你的DLL路径

    // 打开目标进程
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "Could not open process: " << GetLastError() << std::endl;
        return 1;
    }

    // 分配内存
    LPVOID pRemoteDllPath = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (pRemoteDllPath == NULL) {
        std::cerr << "Could not allocate memory in target process: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    // 写入DLL路径
    if (!WriteProcessMemory(hProcess, pRemoteDllPath, (LPVOID)dllPath, strlen(dllPath) + 1, NULL)) {
        std::cerr << "Could not write to target process memory: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 获取LoadLibraryA函数地址
    HMODULE hKernel32 = GetModuleHandleA("Kernel32.dll");
    FARPROC pLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");
    if (pLoadLibraryA == NULL) {
        std::cerr << "Could not get LoadLibraryA address: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 创建远程线程来加载DLL
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryA, pRemoteDllPath, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "Could not create remote thread: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 等待线程执行完成
    WaitForSingleObject(hThread, INFINITE);

    // 清理
    VirtualFreeEx(hProcess, pRemoteDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    std::cout << "DLL injected successfully!" << std::endl;
    return 0;
}
