/* Copyright 2021 DucaPowr Team */
#include <iostream>
#include <string>

#include "./engine.h"
#include "./xboardHandler.h"
#include "./logger.h"
#include "./config.h"

// init debug file
std::ofstream Logger::debugFile(DEBUG_FILE_NAME);

// main
int main() {
    Engine engine;
    xBoardHandler handler(engine);

    handler.init();

    while (true) {
        handler.run();
    }

    return 0;
}
