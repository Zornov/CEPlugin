#include "CLog.hpp"
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <mutex>

static CLog g_Log{};

auto CLog::Initialize() -> void {
    AllocConsole();

    SetConsoleCP( CP_UTF8 );
    SetConsoleOutputCP( CP_UTF8 );

    HANDLE hConsoleOut = GetStdHandle( STD_OUTPUT_HANDLE );
    const int fdOut = _open_osfhandle( reinterpret_cast<intptr_t>(hConsoleOut), _O_TEXT );
    COutputHandle = _fdopen( fdOut, "w" );

    HANDLE hConsoleErr = GetStdHandle( STD_ERROR_HANDLE );
    const int fdErr = _open_osfhandle( reinterpret_cast<intptr_t>(hConsoleErr), _O_TEXT );
    CErrorHandle = _fdopen( fdErr, "w" );

    freopen_s( &COutputHandle, "CONOUT$", "w", stderr );
    freopen_s( &CErrorHandle, "CONOUT$", "w", stdout );

    std::cout.clear();
    std::cerr.clear();
}

auto CLog::Destroy() -> void {
    if ( CErrorHandle ) {
        fflush( CErrorHandle );
        fclose( CErrorHandle );
        CErrorHandle = nullptr;
    }

    if ( COutputHandle ) {
        fflush( COutputHandle );
        fclose( COutputHandle );
        COutputHandle = nullptr;
    }

    FreeConsole();
}

auto CLog::AddLog( const char* fmt, ... ) -> void {
    std::lock_guard lock( m_Lock );

    char buff[4096] = {};

    va_list args;
    va_start( args, fmt );
    vsnprintf( buff, sizeof( buff ) - 1, fmt , args );
    va_end( args );

    printf( "%s", buff );
}

auto GetLog() -> CLog* {
    return &g_Log;
}