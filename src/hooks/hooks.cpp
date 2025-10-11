#include "hooks.h"

#include <cpr/cpr.h>
#include <json.hpp>

#include "base64.hpp"

using json = nlohmann::json;

struct ProcessData {
    unsigned long pid{};
    std::string name;

    void fromJson(const json& j) {
        pid = std::stoul(j["pid"].get<std::string>());
        name = j["exe"].get<std::string>();
    }

    void toProcessEntry(PROCESSENTRY32* lppe) const {
        if (!lppe) return;
        ZeroMemory(lppe, sizeof(PROCESSENTRY32));
        lppe->dwSize = sizeof(PROCESSENTRY32);
        lppe->th32ProcessID = static_cast<DWORD>(pid);
        lppe->cntUsage = 1;
        lppe->cntThreads = 1;
        lppe->pcPriClassBase = 8;
        strncpy_s(lppe->szExeFile, sizeof(lppe->szExeFile), name.c_str(), _TRUNCATE);
    }
};

std::vector<ProcessData> g_processes_read;
std::mutex g_mutex;
size_t g_index = 0;

namespace hooks {
    HANDLE WINAPI hk_CreateToolhelp32Snapshot(DWORD, DWORD) {
        auto async_response = cpr::GetAsync(
            cpr::Url{"http://" + serverIp + "/processes"},
            cpr::Timeout{2000}
        );

        try {
            cpr::Response r = async_response.get();
            json j = json::parse(r.text);

            std::vector<ProcessData> tmp;
            for (auto& item : j) {
                ProcessData p;
                p.fromJson(item);
                tmp.push_back(p);
            }

            std::lock_guard lock(g_mutex);
            std::swap(g_processes_read, tmp);
            g_index = 0;
        } catch (const std::exception& e) {
            printf("[!] Error fetching processes: %s\n", e.what());
        }

        return reinterpret_cast<HANDLE>(0x66);
    }

    BOOL WINAPI hk_Process32First(HANDLE, LPPROCESSENTRY32 lppe) {
        std::lock_guard lock(g_mutex);
        if (g_processes_read.empty()) return FALSE;

        g_index = 0;
        g_processes_read[g_index].toProcessEntry(lppe);
        return TRUE;
    }

    BOOL WINAPI hk_Process32Next(HANDLE, LPPROCESSENTRY32 lppe) {
        std::lock_guard lock(g_mutex);
        if (g_index + 1 >= g_processes_read.size()) return FALSE;

        ++g_index;
        g_processes_read[g_index].toProcessEntry(lppe);
        return TRUE;
    }

    HANDLE WINAPI hk_OpenProcess(DWORD ,BOOL, DWORD) {
        return reinterpret_cast<HANDLE>(0x69);
    }

    BOOL WINAPI hk_ReadProcessMemory(
        const HANDLE hProcess,
        const LPCVOID lpBaseAddress,
        const LPVOID lpBuffer,
        const SIZE_T nSize,
        SIZE_T* lpNumberOfBytesRead
    ) {
        printf("ReadProcessMemory called: hProcess=%p, lpBaseAddress=%p, lpBuffer=%p, nSize=%llu\n",
               hProcess, lpBaseAddress, lpBuffer, nSize);
        return ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
    }

    BOOL WINAPI hk_WriteProcessMemory(
        HANDLE hProcess,
        LPVOID lpBaseAddress,
        LPCVOID lpBuffer,
        SIZE_T nSize,
        SIZE_T* lpNumberOfBytesWritten
    ) {
        return WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
    }
}