#include "hooks.h"

// #include <cpr/cpr.h>
#include <json.hpp>
#include <string>

using json = nlohmann::json;

namespace hooks {

    HANDLE WINAPI hk_OpenProcess(
        DWORD,
        BOOL,
        const DWORD dwProcessId
    ) {
        // const auto response = cpr::Post(
        //     cpr::Url{"http://" + serverIp + "/open-process"},
        //     cpr::Payload{{"pid", std::to_string(dwProcessId)}},
        //     cpr::Timeout{2000}
        // );
        // printf("[*] OpenProcess called: %s", response.text.c_str());
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
        // try {
        //     const auto addr = reinterpret_cast<uint64_t>(lpBaseAddress);
        //     const auto* bytes = static_cast<const unsigned char*>(lpBuffer);
        //
        //     std::string hexData;
        //     hexData.reserve(nSize * 2);
        //     static auto hex = "0123456789ABCDEF";
        //     for (size_t i = 0; i < nSize; ++i) {
        //         hexData.push_back(hex[(bytes[i] >> 4) & 0xF]);
        //         hexData.push_back(hex[bytes[i] & 0xF]);
        //     }
        //
        //     const json payload = {
        //         {"address", addr},
        //         {"data", hexData}
        //     };
        //
        //     auto response = cpr::Post(
        //         cpr::Url{"http://" + serverIp + "/write-memory"},
        //         cpr::Body{payload.dump()},
        //         cpr::Header{{"Content-Type", "application/json"}},
        //         cpr::Timeout{3000}
        //     );
        //
        //     if (response.status_code != 200) {
        //         printf("[!] WriteProcessMemory failed: HTTP %d\n", response.status_code);
        //         return FALSE;
        //     }
        //
        //     auto j = json::parse(response.text);
        //     if (j.contains("error")) {
        //         printf("[!] Server write error: %s\n", j["error"].get<std::string>().c_str());
        //         return FALSE;
        //     }
        //
        //     if (lpNumberOfBytesWritten)
        //         *lpNumberOfBytesWritten = nSize;
        //
        //     return TRUE;
        // } catch (const std::exception& e) {
        //     printf("[!] Exception in hk_WriteProcessMemory: %s\n", e.what());
        //     return FALSE;
        // }
        return FALSE;
    }
}
