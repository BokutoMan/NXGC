#include <Windows.h>
#include <stdio.h>

using namespace std;

void print_time() {
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    printf("there is print_time: %02d:%02d:%02d\n", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
}

void HOOK_print_time() {
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    printf("there is HOOK_print_time: %02d:%02d:%02d\n", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
}

// Inline Hook的函数
BOOL InlineHookGetLocalTime()
{
    // 计算相对跳转偏移
    uintptr_t offset = (uintptr_t)HOOK_print_time - ((uintptr_t)print_time + 5);

    // 修改函数的前几个字节为汇编跳转指令
    DWORD oldProtect;
    VirtualProtect(print_time, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    // 构造跳转指令（JMP opcode）
    *(BYTE*)print_time = 0xE9;
    *(uintptr_t*)((BYTE*)print_time + 1) = offset;

    // 恢复保护属性
    VirtualProtect(print_time, 5, oldProtect, &oldProtect);

    return TRUE;
}

int main() {

    printf("挂钩子前运行print_time");
    print_time();
    
    // 进行Inline Hook
    if (!InlineHookGetLocalTime()) {
        printf("Failed to hook GetLocalTime\n");
        return 1;
    }

    // 再次调用print_time，此时会调用被hook的函数
    printf("挂钩子后运行print_time");
    print_time();
    printf("over");
    
    return 0;
}
