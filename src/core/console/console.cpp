#include "console.h"
#include "../logging/logging.h"

#include <windows.h>
#include <cstdio>
#include <iostream>
#include <mutex>

namespace {
    std::once_flag g_console_once;
}

void SetupConsoleOnce() {
    std::call_once(g_console_once, [] {
        if (!AllocConsole()) {
            log_warn("Failed to allocate console.");
            return;
        }
        FILE* fp = nullptr;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$",  "r", stdin);
        std::ios::sync_with_stdio(true);
        SetConsoleTitleA("Dma Plugin Console");
        log_note("Console initialized.");
    });
}

std::string prompt(const char* text, const char* defval) {
    std::string line;
    std::cout << "[*] " << text << " [" << defval << "]: ";
    std::getline(std::cin, line);
    if (line.empty()) line = defval;
    return line;
}