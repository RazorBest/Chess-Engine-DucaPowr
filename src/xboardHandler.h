/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdio.h>
#include <iostream>
#include <sstream>

#include "./engine.h"
#include "./logger.h"

class xBoardHandler {
 private:
    Engine& _engine;
    Logger _logger;

    bool observing = true;

    void engineMove();
    std::vector<std::string> quotes;
    std::string getResignationString();
 public:
    explicit xBoardHandler(Engine& engine);
    void init();
    void run();
};
