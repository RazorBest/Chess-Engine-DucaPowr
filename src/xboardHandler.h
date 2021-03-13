#pragma once

#include <iostream>
#include <stdio.h>
#include <sstream>
#include "engine.h"
#include "logger.h"

class xBoardHandler {
    private:
        Engine engine;
        Logger logger;

        bool engineRunning = true;
    public:
        xBoardHandler(Engine& engine);
        void init();
        void run();
};