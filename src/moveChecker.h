/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdint.h>

#include "./board.h"

class MoveChecker {
 public:
    explicit MoveChecker(Board& board);

    bool isLegal(uint16_t move);
 private:
    Board& _board;

    bool isEnPassant(uint16_t move);
};
