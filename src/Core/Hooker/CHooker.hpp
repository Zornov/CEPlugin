#pragma once

#include <vector>
#include <Windows.h>
#include <cepluginsdk.h>

struct HookData {
    const char* m_pName = nullptr;
    void* m_pTarget = nullptr;
    void* m_pDetour = nullptr;
    void** m_pOriginal = nullptr;
    bool m_bSkipIfNotFound = false;
    bool m_bSkipError = false;
};

class CHooker final {
public:
    auto Initialize() -> bool;
    auto InstallSecondHook( PExportedFunctions functions ) -> bool;
    auto Destroy() -> void;


private:
    auto InstallHooks() -> bool;

    template<typename HookT>
    static auto SetInternalHook(void* field_ptr, HookT hook, const char* name = nullptr) noexcept -> uintptr_t;

    bool m_bInitialized = false;
    std::vector<HookData> m_Hooks;
};

auto GetHooker() -> CHooker*;