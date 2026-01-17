#include "CHooker.hpp"

#include <Common/Log/CLog.hpp>
#include "Methods/CreateToolhelp32Snapshot_Hook.hpp"

static CHooker g_CHooker{};

auto CHooker::Initialize() -> bool {
    if ( m_bInitialized )
        return false;

    LOG("[info] Initializing hooks!\n");

    m_bInitialized = true;
    return true;
}

auto CHooker::InstallSecondHook( const PExportedFunctions functions ) -> bool {
    m_Hooks = {
        {
            "CheatEngine::CreateToolhelp32Snapshot",
            &functions->CreateToolhelp32Snapshot,
            reinterpret_cast<void*>(Hook_CreateToolhelp32Snapshot),
            reinterpret_cast<void**>(&CreateToolhelp32Snapshot_o),
            false,
            false
        }
    };

    return InstallHooks();
}

auto CHooker::InstallHooks() -> bool {
    for (auto& [m_pName, m_pTarget, m_pDetour, m_pOriginal, m_bSkipIfNotFound, m_bSkipError] : m_Hooks) {
        if ( !m_pTarget ) {
            if ( !m_bSkipError )
                LOG( "[error] Hook target is null -> '%s'\n", m_pName ? m_pName : "Unknown" );

            if ( !m_bSkipIfNotFound )
                return false;

            continue;
        }

        const uintptr_t old = SetInternalHook(m_pTarget, m_pDetour, m_pName);

        if ( old == 0 ) {
            if ( !m_bSkipError )
                LOG( "[error] Failed to set internal hook -> '%s'\n", m_pName ? m_pName : "Unknown" );

            if ( !m_bSkipIfNotFound )
                return false;

            continue;
        }

        if ( m_pOriginal ) {
            *m_pOriginal = reinterpret_cast<void*>(old);
        }

        LOG( "[info] Hooked -> '%s'\n", m_pName ? m_pName : "Unknown" );
    }

    m_Hooks.clear();
    return true;
}

template<typename HookT>
auto CHooker::SetInternalHook(void* field_ptr, HookT hook, const char* name) noexcept -> uintptr_t {
    if (!field_ptr) {
        if (name)
            LOG( "[error] field_ptr == nullptr for %s\n", name );
        return 0;
    }

    const auto target = static_cast<uintptr_t*>(field_ptr);
    const uintptr_t old = *target;

    if (old == 0) {
        if (name)
            LOG("[error] Original function pointer is null for %s\n", name);
        return 0;
    }

    *target = reinterpret_cast<uintptr_t>(hook);

    return old;
}

auto CHooker::Destroy() -> void {
    if ( !m_bInitialized )
        return;

    m_bInitialized = false;
    LOG("[info] Hooks destroyed!\n");
}

auto GetHooker() -> CHooker* {
    return &g_CHooker;
}