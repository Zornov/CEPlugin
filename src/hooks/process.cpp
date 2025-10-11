#include "hooks.h"

#include <cpr/cpr.h>
#include <json.hpp>

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
            if (r.text.empty()) {
                return INVALID_HANDLE_VALUE;
            }
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
            printf("[!] Error fetching processes: %s", e.what());
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
}