#pragma once

#include <mutex>

class CLog final {
public:
    auto Initialize() -> void;
    auto Destroy() -> void;
    auto AddLog( const char* fmt, ... ) -> void;

private:
    std::recursive_mutex m_Lock;

    FILE* COutputHandle = nullptr;
    FILE* CErrorHandle = nullptr;
};

auto GetLog() -> CLog*;

#define LOG( fmt, ... ) GetLog()->AddLog( fmt, __VA_ARGS__ )