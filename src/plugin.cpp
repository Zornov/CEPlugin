#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#include <windows.h>
#include <cstdio>
#include <iostream>
#include <string>

#include "hooks/hooks.h"

extern "C" {
    #include "cepluginsdk.h"
}

#pragma comment(lib, "Ws2_32.lib")

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
uintptr_t set_hook(void* field_ptr, HookT hook, const char* name = nullptr) noexcept {
    if (!field_ptr) {
        if (name) std::cerr << "[!] hook: field_ptr == nullptr for " << name << std::endl;
        return 0;
    }

    const auto target = static_cast<uintptr_t*>(field_ptr);
    const uintptr_t old = *target;

    *target = reinterpret_cast<uintptr_t>(hook);

    if (name) {
        std::cout << "[+] Hooked " << name
                  << " (old: 0x" << std::hex << old
                  << " new: 0x" << reinterpret_cast<uintptr_t>(hook) << std::dec << ")" << std::endl;
    }

    return old;
}


BOOL __stdcall CEPlugin_GetVersion(const PPluginVersion pv, int) {
    if (!pv) return FALSE;
    pv->version = CESDK_VERSION;
    pv->pluginname = "DMA Plugin";
    return TRUE;
}

BOOL __stdcall CEPlugin_InitializePlugin(const PExportedFunctions ef, int) {
    if (!ef) return FALSE;

    SetupConsole();
    std::string serverIp;
    std::string serverPort;

    std::cout << "[*] Enter server IP: ";
    std::cin >> serverIp;
    std::cout << "[*] Enter server port: ";
    std::cin >> serverPort;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[!] WSAStartup failed" << std::endl;
        return FALSE;
    }
    std::cout << "[+] WinSock initialized"  << std::endl;

    addrinfo hints{};
    addrinfo* result = nullptr;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(serverIp.c_str(), serverPort.c_str(), &hints, &result) != 0) {
        std::cerr << "[!] getaddrinfo failed with error: " << WSAGetLastError() << std::endl;
        CEPlugin_DisablePlugin();
        return FALSE;
    }

    std::cout << "[+] Address resolved successfully" << std::endl;

    hooks::server = socket(result->ai_family, result->ai_socktype,
            result->ai_protocol);

    if (hooks::server == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        CEPlugin_DisablePlugin();
        return 1;
    }

    freeaddrinfo(result);

    if (connect(hooks::server, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
        std::cerr << "connect failed with error: " << WSAGetLastError() << std::endl;
        closesocket(hooks::server);
        CEPlugin_DisablePlugin();
        return 1;
    }

    const auto sendbuf = "this is a test";
    if (send(hooks::server, sendbuf, static_cast<int>(strlen(sendbuf)), 0) ==  SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(hooks::server);
        CEPlugin_DisablePlugin();
        return 1;
    }

    set_hook(ef->CreateToolhelp32Snapshot, &hooks::hk_CreateToolhelp32Snapshot, "CreateToolhelp32Snapshot");
    set_hook(ef->Process32First, &hooks::hk_Process32First, "Process32First");
    set_hook(ef->Process32Next, &hooks::hk_Process32Next, "Process32Next");
    set_hook(ef->OpenProcess, &hooks::hk_OpenProcess, "OpenProcess");

    set_hook(ef->ReadProcessMemory, &hooks::hk_ReadProcessMemory, "ReadProcessMemory");
    set_hook(ef->WriteProcessMemory, &hooks::hk_WriteProcessMemory, "WriteProcessMemory");

    return TRUE;
}

BOOL __stdcall CEPlugin_DisablePlugin() {
    std::cout << "[+] Disabling plugin" << std::endl;
    WSACleanup();
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}