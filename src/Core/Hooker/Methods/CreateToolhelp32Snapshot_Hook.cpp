#include "CreateToolhelp32Snapshot_Hook.hpp"

auto Hook_CreateToolhelp32Snapshot(
    DWORD dwFlags,
    DWORD th32ProcessID
) -> HANDLE {

    return CreateToolhelp32Snapshot_o(
        dwFlags,
        th32ProcessID
    );
}