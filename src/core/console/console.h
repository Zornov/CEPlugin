#pragma once
#include <string>

void SetupConsoleOnce();
std::string prompt(const char* text, const char* defval);