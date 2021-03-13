#pragma once

#include <fstream>

class Logger {
    public:
        static std::ofstream debug_file;

        void log(std::string msg, std::string type);
        void info(std::string msg);
        void error(std::string msg);
};