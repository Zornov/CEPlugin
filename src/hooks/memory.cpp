#include "hooks.h"

#include <cpr/cpr.h>
#include <json.hpp>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;

namespace hooks {

    HANDLE WINAPI hk_OpenProcess(
        DWORD,
        BOOL,
        const DWORD dwProcessId
    ) {
        const auto response = cpr::Post(
            cpr::Url{"http://" + serverIp + "/open-process"},
            cpr::Payload{{"pid", std::to_string(dwProcessId)}},
            cpr::Timeout{2000}
        );
        printf("[*] OpenProcess called: %s", response.text.c_str());
        return reinterpret_cast<HANDLE>(0x69);
    }

    BOOL WINAPI hk_ReadProcessMemory(
        HANDLE,
        const LPCVOID lpBaseAddress,
        const LPVOID lpBuffer,
        const SIZE_T nSize,
        SIZE_T* lpNumberOfBytesRead
    ) {
        printf("[*] ReadProcessMemory called: lpBaseAddress=%p, lpBuffer=%p, nSize=%llu\n",
               lpBaseAddress, lpBuffer, static_cast<unsigned long long>(nSize));
        return FALSE;
    }


    BOOL WINAPI hk_WriteProcessMemory(
        HANDLE hProcess,
        LPVOID lpBaseAddress,
        LPCVOID lpBuffer,
        SIZE_T nSize,
        SIZE_T* lpNumberOfBytesWritten
    ) {
        return WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
    }
}
