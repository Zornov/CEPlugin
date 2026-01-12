#pragma once

#include <windows.h>
#include <tlhelp32.h>

using CreateToolhelp32Snapshot_t =
    HANDLE (WINAPI*)(
        DWORD,
        DWORD
    );

auto Hook_CreateToolhelp32Snapshot(
    DWORD dwFlags,
    DWORD th32ProcessID
) -> HANDLE;

inline CreateToolhelp32Snapshot_t CreateToolhelp32Snapshot_o = nullptr;