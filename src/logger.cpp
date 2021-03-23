#include "logger.h"

void Logger::raw(std::string msg) {
    debug_file << msg << std::endl;
}

void Logger::log(std::string msg, std::string type) {
    debug_file << '[' << type << ']' << " -> " << msg << std::endl;
}

void Logger::info(std::string msg) {
    log(msg, "INFO");
}

void Logger::error(std::string msg) {
    log(msg, "ERROR");
}

void Logger::logU16(uint16_t x) {
    debug_file << "[INFO] -> " << x << std::endl;
}

void Logger::logBB(uint64_t a) {
    std::bitset<64> x(a);
    debug_file << x << '\n';
    
}
