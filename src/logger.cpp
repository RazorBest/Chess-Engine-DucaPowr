#include "logger.h"

void Logger::log(std::string msg, std::string type) {
    debug_file << '[' << type << ']' << " -> " << msg << std::endl;
}

void Logger::info(std::string msg) {
    log(msg, "INFO");
}

void Logger::error(std::string msg) {
    log(msg, "ERROR");
}