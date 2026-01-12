#pragma once

#include <cepluginsdk.h>
#include <mutex>
#include <string>

class CPluginLauncher final {
public:
    auto OnInitializePlugin( PExportedFunctions exportedFunctions, int pluginId ) const -> void;
    auto OnDisablePlugin() -> void;

    auto GetVersion( PPluginVersion pv, int sizeOfPluginVersion ) const -> void;
private:
    std::string m_PluginName = "DMA Plugin";
    int m_Version = CESDK_VERSION;

	std::once_flag m_bDestroyed;
};

auto GetPluginLauncher() -> CPluginLauncher*;
