#include <windows.h>
#include <cstdio>
#include <iostream>
#include <string>

#include "hooks/hooks.h"

extern "C" {
    #include "cepluginsdk.h"
}

void SetupConsole() {
    AllocConsole();
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$",  "r", stdin);
    std::ios::sync_with_stdio(true);

    SetConsoleTitleA("Dma Plugin Console");
    std::cout << "[*] Console initialized.\n";
}

template<typename HookT>
uintptr_t hook(void* field_ptr, HookT hook, const char* name = nullptr) noexcept {
    if (!field_ptr) {
        if (name) std::cerr << "[!] hook: field_ptr == nullptr for " << name << '\n';
        return 0;
    }

    const auto target = static_cast<uintptr_t*>(field_ptr);
    const uintptr_t old = *target;

    *target = reinterpret_cast<uintptr_t>(hook);

    if (name) {
        std::cout << "[*] Hooked " << name
                  << " (old: 0x" << std::hex << old
                  << " new: 0x" << reinterpret_cast<uintptr_t>(hook) << std::dec << ")\n";
    }

    return old;
}


BOOL __stdcall CEPlugin_GetVersion(const PPluginVersion pv, int) {
    if (!pv) return FALSE;
    pv->version = CESDK_VERSION;
    pv->pluginname = "DMA Plugin";
    return TRUE;
}

BOOL __stdcall CEPlugin_InitializePlugin(const PExportedFunctions ef, int pluginid) {
    if (!ef) return FALSE;

    SetupConsole();

    std::cout << "[*] Enter server IP with port: ";
    std::getline(std::cin, hooks::serverIp);
    std::cout << "[*] Server IP: " << hooks::serverIp << "\n";

    hook(ef->CreateToolhelp32Snapshot, &hooks::hk_CreateToolhelp32Snapshot, "CreateToolhelp32Snapshot");
    hook(ef->Process32First, &hooks::hk_Process32First, "Process32First");
    hook(ef->Process32Next, &hooks::hk_Process32Next, "Process32Next");
    hook(ef->OpenProcess, &hooks::hk_OpenProcess, "OpenProcess");

    hook(ef->ReadProcessMemory, &hooks::hk_ReadProcessMemory, "ReadProcessMemory");
    hook(ef->WriteProcessMemory, &hooks::hk_WriteProcessMemory, "WriteProcessMemory");

    return TRUE;
}

BOOL __stdcall CEPlugin_DisablePlugin() {
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD /*ul_reason_for_call*/, LPVOID /*lpReserved*/) {
    return TRUE;
}