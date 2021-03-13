#pragma once

#include <iostream>
#include <sstream>
#include "chess.h"
#include "logger.h"

class xBoardHandler {
    private:
        Chess chess;
        Logger logger;

        bool engineRunning = true;
    public:
        xBoardHandler(Chess& chess);
        void init();
        void run();
};