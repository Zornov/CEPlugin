#include "hooks.h"

#include <cpr/cpr.h>
#include <json.hpp>
#include <base64.hpp>
#include <fmt/core.h>
#include <string>
#include <iostream>

using json = nlohmann::json;

namespace hooks {

    HANDLE WINAPI hk_OpenProcess(
        DWORD,
        BOOL,
        const DWORD dwProcessId
    ) {
        const auto response = cpr::Post(
            cpr::Url{"http://" + serverIp + "/open-process"},
            cpr::Payload{{"pid", std::to_string(dwProcessId)}},
            cpr::Timeout{2000}
        );
        printf("[*] OpenProcess called: %s", response.text.c_str());
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

            auto response = cpr::Post(
                cpr::Url{"http://" + serverIp + "/read-memory"},
                cpr::Body{payload.dump()},
                cpr::Header{{"Content-Type", "application/json"}},
                cpr::Timeout{3000}
            );

            if (response.status_code != 200) {
                printf("[!] Failed to read memory: HTTP %d\n", response.status_code);
                return FALSE;
            }

            auto j = json::parse(response.text);
            const auto hexData = j["data"].get<std::string>();

            for (size_t i = 0; i < nSize && i*2 + 1 < hexData.size(); ++i) {
                std::string byteStr = hexData.substr(i*2, 2);
                static_cast<unsigned char*>(lpBuffer)[i] = static_cast<unsigned char>(std::stoul(byteStr, nullptr, 16));
            }

            if (lpNumberOfBytesRead)
                *lpNumberOfBytesRead = nSize;

            return TRUE;

        } catch (const std::exception& e) {
            printf("[!] Exception in hk_ReadProcessMemory: %s\n", e.what());
            return FALSE;
        }
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
