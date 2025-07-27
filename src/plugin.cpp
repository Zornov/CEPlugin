
#include <windows.h>
#include <cstdio>

extern "C" {
    #include "cepluginsdk.h"
}

ExportedFunctions Exported;

BOOL __stdcall hk_read_process_memory(const HANDLE hProcess, const LPCVOID lpBaseAddress, const LPVOID lpBuffer, const SIZE_T nSize, SIZE_T* lpNumberOfBytesRead) {
    printf("[DMA] ReadProcessMemory: process=%p, address=0x%p, size=%zu bytes\n",
           hProcess, lpBaseAddress, nSize);
    return ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
}

BOOL __stdcall hk_write_process_memory(const HANDLE hProcess, const LPVOID lpBaseAddress, const LPCVOID lpBuffer, const SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten) {
    printf("[DMA] WriteProcessMemory: process=%p, address=0x%p, size=%zu bytes\n",
           hProcess, lpBaseAddress, nSize);
    return WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
}

HANDLE __stdcall hk_open_process(const DWORD dwDesiredAccess, const BOOL bInheritHandle, const DWORD dwProcessId) {
    printf("[DMA] OpenProcess: PID=%lu, access=0x%lx\n",
           dwProcessId, dwDesiredAccess);
    return OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
}

BOOL __stdcall hk_virtual_query_ex(const HANDLE hProcess, const LPCVOID lpAddress, const PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength) {
    printf("[DMA] VirtualQueryEx: process=%p, address=0x%p\n",
           hProcess, lpAddress);
    return VirtualQueryEx(hProcess, lpAddress, lpBuffer, dwLength);
}

BOOL APIENTRY DllMain(HANDLE hModule, const DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            AllocConsole();
            freopen("CONOUT$", "w", stdout);
            printf("[DMA] Plugin loaded successfully\n");
            break;
        case DLL_PROCESS_DETACH:
            printf("[DMA] Plugin unloading...\n");
            break;
        case DLL_THREAD_ATTACH:
            printf("[DMA] New thread attached\n");
            break;
        case DLL_THREAD_DETACH:
            printf("[DMA] Thread detached\n");
            break;
    }
    return TRUE;
}

BOOL __stdcall CEPlugin_GetVersion(PPluginVersion pv, int sizeofpluginversion) {
    if (!pv) return FALSE;
    pv->version = CESDK_VERSION;
    pv->pluginname = "DMA Operations Monitor";
    return TRUE;
}

BOOL __stdcall CEPlugin_InitializePlugin(PExportedFunctions ef, int pluginid) {
    if (!ef) {
        printf("[DMA] ERROR: Invalid exported functions pointer\n");
        return FALSE;
    }

    printf("[DMA] Initializing plugin...\n");
    printf("[DMA] Plugin ID: %d\n", pluginid);

    Exported = *ef;

    printf("[DMA] Installing memory operation hooks...\n");

    *reinterpret_cast<FARPROC *>(ef->ReadProcessMemory) = reinterpret_cast<FARPROC>(hk_read_process_memory);
    printf("[DMA] ReadProcessMemory hook installed\n");

    *static_cast<FARPROC *>(ef->WriteProcessMemory) = reinterpret_cast<FARPROC>(hk_write_process_memory);
    printf("[DMA] WriteProcessMemory hook installed\n");

    *static_cast<FARPROC *>(ef->OpenProcess) = reinterpret_cast<FARPROC>(hk_open_process);
    printf("[DMA] OpenProcess hook installed\n");

    *static_cast<FARPROC *>(ef->VirtualQueryEx) = reinterpret_cast<FARPROC>(hk_virtual_query_ex);
    printf("[DMA] VirtualQueryEx hook installed\n");

    printf("[DMA] Initialization complete\n");
    return TRUE;
}

BOOL __stdcall CEPlugin_DisablePlugin() {
    printf("[DMA] Plugin shutdown initiated\n");
    printf("[DMA] Cleaning up...\n");
    FreeConsole();
    printf("[DMA] Plugin disabled successfully\n");
    return TRUE;
}