#pragma once

#include <cepluginsdk.h>
#include <string>

class CPluginLauncher final {
public:
    auto OnInitializePlugin( PExportedFunctions exportedFunctions, int pluginId ) const -> void;
    static auto OnDisablePlugin() -> void;

    auto GetVersion( PPluginVersion pv, int sizeOfPluginVersion ) const -> void;
private:
    std::string m_PluginName = "DMA Plugin";
    int m_Version = CESDK_VERSION;
};

auto GetPluginLauncher() -> CPluginLauncher*;
