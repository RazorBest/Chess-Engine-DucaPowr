/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdint.h>

#include "./board.h"
#include "./utils.h"

class MoveChecker {
 public:
    explicit MoveChecker(Board& board);

    bool isLegal(uint16_t move, uint16_t* moves, uint16_t moves_len);
 private:
    Board& _board;

    // bool isEnPassant(uint16_t move);
};
