/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdint.h>

#include "./board.h"
#include "./logger.h"
#include "./constants.h"

/**
 * Generate all possible moves.
 * Moves are encoded as:
 * bits 0-5 source square
 * bits 6-11 destination square
 * bits 12-13 promotion (0 - rook, 1 - knight, 2 - bishop, 3 - queen)
 * bits 14-15 flags (0 - none, 1 - promotion, 2 - en passant, 3 - castling)
*/
class Generator {
 public:
    explicit Generator(Board& board);

    void generateMoves(uint16_t* moves, uint16_t* len);

 private:
    Board& _board;
    Logger _logger;

    void whitePawnMoves(uint16_t* moves, uint16_t* len);
    void blackPawnMoves(uint16_t* moves, uint16_t* len);
    void whitePawnAttacks(uint16_t* moves, uint16_t* len);
    void blackPawnAttacks(uint16_t* moves, uint16_t* len);


    void rookMoves(uint16_t* moves, uint16_t* len);

    void bishopMoves(uint16_t* moves, uint16_t* len);

    void knightMoves(uint16_t* moves, uint16_t* len);

    void queenMoves(uint16_t* moves, uint16_t* len);

    void kingMoves(uint16_t* moves, uint16_t* len);
};
