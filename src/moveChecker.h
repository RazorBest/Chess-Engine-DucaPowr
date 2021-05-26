/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdint.h>

#include "./board.h"
#include "./utils.h"

class MoveChecker {
 public:
    explicit MoveChecker(Board& board);

    bool isLegal(uint16_t move, U64 attackBB);

    bool isCheck(U64 attackBB);
 private:
    Board& _board;

    // bool isEnPassant(uint16_t move);
};


// check if move is legal (din start)
// apply move (is check)
// eval -> for any more optimizations