#pragma once
#include <iostream>

extern "C" {
    #include "cepluginsdk.h"
}

template<typename HookT>
uintptr_t set_hook(void* field_ptr, HookT hook, const char* name = nullptr) noexcept {
    if (!field_ptr) {
        if (name) std::cerr << "[!] hook: field_ptr == nullptr for " << name << std::endl;
        return 0;
    }

    const auto target = static_cast<uintptr_t*>(field_ptr);
    const uintptr_t old = *target;

    *target = reinterpret_cast<uintptr_t>(hook);

    if (name) {
        std::cout << "[+] Hooked " << name
                  << " (old: 0x" << std::hex << old
                  << " new: 0x" << reinterpret_cast<uintptr_t>(hook) << std::dec << ")" << std::endl;
    }

    return old;
}

void SetupHooks(PExportedFunctions ef);