#include "hooker.h"

#include "../../hooks/hooks.h"

void SetupHooks(const PExportedFunctions ef) {
    if (!ef) return;
    set_hook(ef->CreateToolhelp32Snapshot, &hooks::hk_CreateToolhelp32Snapshot, "CreateToolhelp32Snapshot");
    set_hook(ef->Process32First, &hooks::hk_Process32First, "Process32First");
    set_hook(ef->Process32Next, &hooks::hk_Process32Next, "Process32Next");
    set_hook(ef->OpenProcess, &hooks::hk_OpenProcess, "OpenProcess");

    set_hook(ef->ReadProcessMemory, &hooks::hk_ReadProcessMemory, "ReadProcessMemory");
    set_hook(ef->WriteProcessMemory, &hooks::hk_WriteProcessMemory, "WriteProcessMemory");
}