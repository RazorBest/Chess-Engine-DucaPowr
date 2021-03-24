#pragma once

#include <stdint.h>

#include "board.h"
#include "logger.h"
#include "constants.h"

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
        Generator(Board& board);

        void generateMoves(Board board, uint16_t* moves, uint16_t* len);

    private:
        Board board;
        Logger logger;

        void whitePawnMoves(Board board, uint16_t* moves, uint16_t* len);
        void blackPawnMoves(Board board, uint16_t* moves, uint16_t* len);

        void rookMoves(Board board, uint16_t* moves, uint16_t* len);

        void bishopMoves(Board board, uint16_t* moves, uint16_t* len);

        void knightMoves(Board board, uint16_t* moves, uint16_t* len);

        void queenMoves(Board board, uint16_t* moves, uint16_t* len);

        void kingMoves(Board board, uint16_t* moves, uint16_t* len);
};
