/* Copyright 2021 DucaPowr Team */
#include "testGenerator.h"
#include <iostream>

#define DEBUG_FILE_NAME "test.debug"

std::ofstream Logger::debugFile(DEBUG_FILE_NAME);

int main() {
    testGenerator();
}
