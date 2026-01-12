#include "PluginLauncher.hpp"

#include "Common/Log/CLog.hpp"
#include "Core/Hooker/CHooker.hpp"

static CPluginLauncher g_CPluginLauncher{};

auto CPluginLauncher::OnInitializePlugin( PExportedFunctions, int ) const -> void {
    GetLog()->Initialize();

    LOG( "Initializing %s v%d\n", m_PluginName.c_str(), m_Version );

    if ( !GetHooker()->Initialize() ) {
        LOG( "[error] Hooker: Initialize\n" );
        return;
    }

    if ( !GetHooker()->InstallSecondHook() ) {
        LOG( "[error] Hooker: InstallSecondHook\n" );
        return;
    }

}

auto CPluginLauncher::OnDisablePlugin() -> void {
    GetHooker()->Destroy();
    GetLog()->Destroy();
}

auto CPluginLauncher::GetVersion( PPluginVersion pv, int ) const -> void {
    if ( !pv ) return;
    pv->version = m_Version;
    pv->pluginname = m_PluginName.c_str();
}

auto GetPluginLauncher() -> CPluginLauncher* {
    return &g_CPluginLauncher;
}