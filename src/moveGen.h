#pragma once

#include <stdint.h>

#include "board.h"
#include "logger.h"

class Generator {
    /**
     * Generate all possible moves.
     * Moves are encoded as:
     * bits 0-5 source square
     * bits 6-11 destination square
     * bits 12-13 promotion (0 - rook, 1 - knight, 2 - bishop, 3 - queen)
     * bits 14-15 flags (0 - none, 1 - promotion, 2 - en passant, 3 - castling)
    */
   public:
        void generateMoves(Board board, uint16_t* moves, uint16_t* cnt);

    private:
        Logger logger;

        void pawnMoves(Board board, uint16_t* moves, uint16_t* cnt);

        void rookMoves(Board board, uint16_t* moves, uint16_t* cnt);

        void bishopMoves(Board board, uint16_t* moves, uint16_t* cnt);

        void knightMoves(Board board, uint16_t* moves, uint16_t* cnt);

        void queenMoves(Board board, uint16_t* moves, uint16_t* cnt);

        void kingMoves(Board board, uint16_t* moves, uint16_t* cnt);
};
