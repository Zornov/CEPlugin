#include <ws2tcpip.h>

#include <windows.h>
#include <boost/asio.hpp>
#include <string>

#include "core/logging/logging.h"
#include "core/console/console.h"
#include "hooks/hooks.h"
#include "core/hooker/hooker.h"

extern "C" {
    #include "cepluginsdk.h"
}

BOOL __stdcall CEPlugin_GetVersion(const PPluginVersion pv, int) {
    if (!pv) return FALSE;
    pv->version = CESDK_VERSION;
    pv->pluginname = "DMA Plugin";
    return TRUE;
}

BOOL __stdcall CEPlugin_InitializePlugin(const PExportedFunctions ef, int) {
    if (!ef) return FALSE;

    SetupConsoleOnce();

    const auto serverIp = prompt("Enter server IP", "192.168.8.103");
    const auto serverPort = prompt("Enter server port", "8765");

    hooks::io_context = std::make_unique<boost::asio::io_context>();

    boost::asio::ip::tcp::resolver resolver(*hooks::io_context);
    const auto endpoints = resolver.resolve(serverIp, serverPort);

    hooks::server = std::make_unique<boost::asio::ip::tcp::socket>(*hooks::io_context);
    boost::asio::connect(*hooks::server, endpoints);

    log_info("Connected to server " + serverIp + ":" + serverPort);

    std::thread([] {
        hooks::io_context->run();
    }).detach();

    SetupHooks(ef);

    return TRUE;
}

BOOL __stdcall CEPlugin_DisablePlugin() {
    log_info("Disabling plugin");
    hooks::server.reset();
    hooks::io_context.reset();
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}