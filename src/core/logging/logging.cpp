#include "logging.h"
#include <iostream>

void log_info(const std::string& msg) {
    std::cout << "[+] " << msg << std::endl;
}
void log_warn(const std::string& msg) {
    std::cout << "[!] " << msg << std::endl;
}
void log_note(const std::string& msg) {
    std::cout << "[*] " << msg << std::endl;
}
void log_error(const std::string& msg) {
    std::cerr << "[-] " << msg << std::endl;
}