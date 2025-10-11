#pragma once
#include <string>
#include <windows.h>
#include <TlHelp32.h>


namespace hooks {
    inline std::string serverIp;

    HANDLE WINAPI hk_CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);
    BOOL WINAPI hk_Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
    BOOL WINAPI hk_Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);

    BOOL WINAPI hk_ReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
    BOOL WINAPI hk_WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten);

    HANDLE WINAPI hk_OpenProcess(
        DWORD dwDesiredAccess,
        BOOL bInheritHandle,
        DWORD dwProcessId
    );
}
