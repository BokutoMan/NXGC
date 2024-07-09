#include <windows.h>
#include "./include/detours.h"

#pragma comment(lib, "./lib.X64/detours.lib")

// Original MessageBoxW function pointer
typedef int (WINAPI* MessageBoxW_t)(HWND, LPCWSTR, LPCWSTR, UINT);
MessageBoxW_t RealMessageBoxW = nullptr;

// Hook function
int WINAPI HookedMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    // Modify the message box text
    return RealMessageBoxW(hWnd, L"Hooked!", lpCaption, uType);
}

void HookIAT() {
    // Get the handle of the current module
    HMODULE hModule = GetModuleHandle(NULL);

    // Get the import address table
    ULONG size;
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size);

    for (; importDesc->Name; importDesc++) {
        PSTR pszModName = (PSTR)((PBYTE)hModule + importDesc->Name);
        if (_stricmp(pszModName, "user32.dll") == 0) {
            // Get the original address of MessageBoxW
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + importDesc->FirstThunk);
            for (; pThunk->u1.Function; pThunk++) {
                PROC* ppfn = (PROC*)&pThunk->u1.Function;
                if (*ppfn == (PROC)GetProcAddress(GetModuleHandle(L"user32.dll"), "MessageBoxW")) {
                    // Save the original function pointer
                    RealMessageBoxW = (MessageBoxW_t)*ppfn;
                    // Detour (hook) the function
                    DetourTransactionBegin();
                    DetourUpdateThread(GetCurrentThread());
                    DetourAttach((PVOID*)ppfn, HookedMessageBoxW);
                    DetourTransactionCommit();
                    break;
                }
            }
            break;
        }
    }
}

int main() {
    // Hook the IAT
    HookIAT();

    // Test the hook
    MessageBoxW(NULL, L"Original", L"Test", MB_OK);

    return 0;
}
