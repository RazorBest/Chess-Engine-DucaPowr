#pragma once

#include <stdint.h>

#include "board.h"

class MoveChecker {
    public:
        MoveChecker(Board& board);

        bool isLegal(uint16_t move);
    private:
        Board board;

        bool isEnPassant(uint16_t move);
};