#pragma once

#include <fstream>
#include <stdint.h>
#include <bitset>

class Logger {
    public:
        static std::ofstream debug_file;

        void raw(std::string msg);
        void log(std::string msg, std::string type);
        void info(std::string msg);
        void error(std::string msg);
        void logU16(uint16_t x);
        void logBB(uint64_t x);
};