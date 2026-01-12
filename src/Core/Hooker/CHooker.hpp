#pragma once

#include <vector>
#include <Windows.h>

struct HookData {
    const char* m_pName = nullptr;
    void* m_pTarget = nullptr;
    LPVOID m_pDetour = nullptr;
    LPVOID* m_pOriginal = nullptr;
    bool m_bSkipIfNotFound = false;
    bool m_bSkipError = false;
};

class CHooker final {
public:
    auto Initialize() -> bool;
    auto InstallSecondHook() -> bool;
    auto Destroy() -> void;


private:
    auto InstallHooks() -> bool;

    template<typename HookT>
    static auto SetInternalHook(void* field_ptr, HookT hook, const char* name = nullptr) noexcept -> uintptr_t;

    bool m_bInitialized = false;
    std::vector<HookData> m_Hooks;
};

auto GetHooker() -> CHooker*;