#include <windows.h>
#include <cstdio>
#include <iostream>
#include <cpr/cpr.h>

#include "hooks/hooks.h"

extern "C" {
    #include "cepluginsdk.h"
}

void SetupConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
}

BOOL __stdcall CEPlugin_GetVersion(const PPluginVersion pv, int sizeofpluginversion) {
    if (!pv) return FALSE;
    pv->version = CESDK_VERSION;
    pv->pluginname = "ZZS_Minimal";
    return TRUE;
}

BOOL __stdcall CEPlugin_InitializePlugin(const PExportedFunctions ef , int pluginid) {
    if (!ef) return FALSE;

    SetupConsole();

    const auto create_tool_help32 = ef->CreateToolhelp32Snapshot;
    const auto process32_first = ef->Process32First;
    const auto process32_next = ef->Process32Next;

    *static_cast<uintptr_t *>(create_tool_help32) = reinterpret_cast<uintptr_t>(&hooks::hk_CreateToolhelp32Snapshot);
    *static_cast<uintptr_t *>(process32_first) = reinterpret_cast<uintptr_t>(&hooks::hk_Process32First);
    *static_cast<uintptr_t *>(process32_next) = reinterpret_cast<uintptr_t>(&hooks::hk_Process32Next);

    return TRUE;
}

BOOL __stdcall CEPlugin_DisablePlugin() {
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}