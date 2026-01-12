#include <ws2tcpip.h>
#include <windows.h>

extern "C" {
    #include <cepluginsdk.h>
}

BOOL __stdcall CEPlugin_GetVersion(PPluginVersion pv, int) {
    if (!pv) return FALSE;
    pv->version = CESDK_VERSION;
    pv->pluginname = "DMA Plugin";
    return TRUE;
}

BOOL __stdcall CEPlugin_InitializePlugin(PExportedFunctions ef, int) {
    if (!ef) return FALSE;

    return TRUE;
}

BOOL __stdcall CEPlugin_DisablePlugin() {

    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}