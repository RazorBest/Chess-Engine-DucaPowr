/* Copyright 2021 DucaPowr Team */
#pragma once

#define DEBUG 0

#include <stdint.h>
#include <fstream>
#include <bitset>
#include <string>
class Logger {
 public:
    static std::ofstream debugFile;

    void raw(std::string msg);
    void log(std::string msg, std::string type);
    void info(std::string msg);
    void error(std::string msg);
    void logU16(uint16_t x);
    void logBB(uint64_t x);
};
