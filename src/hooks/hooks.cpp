#include "hooks.h"

#include <cpr/cpr.h>
#include <json.hpp>

using json = nlohmann::json;

struct ProcessData {
    unsigned long pid;
    std::string name;

    void fromJson(const nlohmann::json& j) {
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

static std::vector<ProcessData> g_processes;
static size_t g_index = 0;

namespace hooks {

    HANDLE WINAPI hk_CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID) {
        cpr::Response r = cpr::Get(
            cpr::Url{"http://192.168.8.15:5000/processes"},
            cpr::Timeout{2000}
        );
        json j = json::parse(r.text);

        g_processes.clear();
        for (const auto& item : j) {
            ProcessData p;
            p.fromJson(item);
            g_processes.push_back(p);
        }

        g_index = 0;
        return reinterpret_cast<HANDLE>(0x66);
    }

    BOOL WINAPI hk_Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe) {
        if (g_processes.empty()) return FALSE;

        g_index = 0;
        g_processes[g_index].toProcessEntry(lppe);
        return TRUE;
    }

    BOOL WINAPI hk_Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe) {
        if (g_index + 1 >= g_processes.size()) return FALSE;

        ++g_index;
        g_processes[g_index].toProcessEntry(lppe);
        return TRUE;
    }


    DWORD WINAPI hk_OpenProcess(const DWORD pid) {
        cpr::Response r = cpr::Post(
            cpr::Url{"http://192.168.8.15:5000/open-process"},
            cpr::Payload{{"pid", std::to_string(pid)}},
            cpr::Timeout{2000}
        );

        json j = json::parse(r.text);

        printf(r.text.c_str());
        return j.get<>();
    }

    BOOL WINAPI hk_ReadProcessMemory(DWORD hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead) {
        printf("ReadProcessMemory called\n");
        return TRUE;
    }

    BOOL WINAPI hk_WriteProcessMemory(DWORD hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten) {
        printf("WriteProcessMemory called\n");
        return TRUE;
    }

}