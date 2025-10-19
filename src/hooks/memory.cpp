#include "hooks.h"

#include <json.hpp>
#include <string>
#include "../core/logging/logging.h"
#include <boost/asio.hpp>

using json = nlohmann::json;

namespace hooks {

    HANDLE WINAPI hk_OpenProcess(
        DWORD,
        BOOL,
        const DWORD dwProcessId
    ) {
        using boost::asio::ip::tcp;

        if (!server || !server->is_open())
            return reinterpret_cast<HANDLE>(0x69);

        boost::system::error_code ec;

        const json req_json = { {"action", "open-process"}, {"pid", std::to_string(dwProcessId)} };
        std::string req = req_json.dump() + "\n";
        boost::asio::write(*server, boost::asio::buffer(req), ec);
        if (ec) {
            return reinterpret_cast<HANDLE>(0x69);
        }

        boost::asio::streambuf buf;
        boost::asio::read_until(*server, buf, "\n", ec);
        if (ec) {
            return reinterpret_cast<HANDLE>(0x69);
        }

        std::istream is(&buf);
        std::string line;
        std::getline(is, line);

        log_note("OpenProcess response: " + line);

        if (line.empty()) {
            return reinterpret_cast<HANDLE>(0x69);
        }

        return reinterpret_cast<HANDLE>(0x69);
    }

    BOOL WINAPI hk_ReadProcessMemory(
        HANDLE,
        const LPCVOID lpBaseAddress,
        LPVOID lpBuffer,
        SIZE_T nSize,
        SIZE_T* lpNumberOfBytesRead
    ) {
        try {
            auto addr = reinterpret_cast<uint64_t>(lpBaseAddress);

            printf("[*] Reading memory at address: 0x%llX, size: %llu bytes\n", addr, nSize);

            const json payload = {
                {"address", addr},
                {"size", nSize}
            };

            // auto response = cpr::Post(
            //     cpr::Url{"http://" + serverIp + "/read-memory"},
            //     cpr::Body{payload.dump()},
            //     cpr::Header{{"Content-Type", "application/json"}},
            //     cpr::Timeout{3000}
            // );
            //
            // if (response.status_code != 200) {
            //     printf("[!] Failed to read memory: HTTP %d\n", response.status_code);
            //     return FALSE;
            // }
            //
            // auto j = json::parse(response.text);
            // const auto hexData = j["data"].get<std::string>();
            //
            // for (size_t i = 0; i < nSize && i*2 + 1 < hexData.size(); ++i) {
            //     std::string byteStr = hexData.substr(i*2, 2);
            //     static_cast<unsigned char*>(lpBuffer)[i] = static_cast<unsigned char>(std::stoul(byteStr, nullptr, 16));
            // }
            //
            // if (lpNumberOfBytesRead)
            //     *lpNumberOfBytesRead = nSize;

            return TRUE;

        } catch (const std::exception& e) {
            printf("[!] Exception in hk_ReadProcessMemory: %s\n", e.what());
            return FALSE;
        }
    }

    BOOL WINAPI hk_WriteProcessMemory(
        HANDLE,
        LPVOID lpBaseAddress,
        LPCVOID lpBuffer,
        const SIZE_T nSize,
        SIZE_T* lpNumberOfBytesWritten
    ) {
        return FALSE;
    }
}