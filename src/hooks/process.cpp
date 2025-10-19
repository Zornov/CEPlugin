#include <iostream>
#include "hooks.h"
#include <json.hpp>
#include <mutex>
#include <vector>
#include <string>
#include <boost/asio.hpp>

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
        using boost::asio::ip::tcp;

        if (!server || !server->is_open())
            return reinterpret_cast<HANDLE>(0x66);

        boost::system::error_code ec;

        json req_json = { {"action", "processes"} };
        std::string req = req_json.dump() + "\n";
        boost::asio::write(*server, boost::asio::buffer(req), ec);
        if (ec) {
            return reinterpret_cast<HANDLE>(0x66);
        }

        boost::asio::streambuf buf;
        boost::asio::read_until(*server, buf, "\n", ec);
        if (ec) {
            return reinterpret_cast<HANDLE>(0x66);
        }

        std::istream is(&buf);
        std::string line;
        std::getline(is, line);

        if (line.empty())
            return reinterpret_cast<HANDLE>(0x66);

        try {
            json j = json::parse(line);

            std::vector<ProcessData> tmp;
            tmp.reserve(j.size());
            for (auto& item : j) {
                ProcessData p;
                p.fromJson(item);
                tmp.push_back(std::move(p));
            }

            std::lock_guard lock(g_mutex);
            g_processes_read.swap(tmp);
            g_index = 0;
        } catch ([[maybe_unused]] const std::exception& e) {

        }
        return reinterpret_cast<HANDLE>(0x66);
    }

    BOOL WINAPI hk_Process32First(HANDLE, const LPPROCESSENTRY32 lppe) {
        std::lock_guard lock(g_mutex);
        if (g_processes_read.empty()) return FALSE;

        g_index = 0;
        g_processes_read[g_index].toProcessEntry(lppe);
        return TRUE;
    }

    BOOL WINAPI hk_Process32Next(HANDLE, const LPPROCESSENTRY32 lppe) {
        std::lock_guard lock(g_mutex);
        if (g_index + 1 >= g_processes_read.size()) return FALSE;

        ++g_index;
        g_processes_read[g_index].toProcessEntry(lppe);
        return TRUE;
    }
}