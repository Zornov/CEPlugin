#include <ws2tcpip.h>
#include <windows.h>

#include "PluginLauncher/PluginLauncher.hpp"

extern "C" {
    #include <cepluginsdk.h>
}

BOOL __stdcall CEPlugin_GetVersion( PPluginVersion pluginVersion, int sizeOfPluginVersion ) {
    if (!pluginVersion) return FALSE;

    pluginVersion->version = CESDK_VERSION;
    pluginVersion->pluginname = "DMA Plugin";

    return TRUE;
}

BOOL __stdcall CEPlugin_InitializePlugin( PExportedFunctions exportedFunctions, int pluginId ) {
    if (!exportedFunctions) return FALSE;

    GetPluginLauncher()->OnInitializePlugin( exportedFunctions, pluginId );

    return TRUE;
}

BOOL __stdcall CEPlugin_DisablePlugin() {

    GetPluginLauncher()->OnDisablePlugin();

    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}