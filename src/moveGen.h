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

    U64 firstRankAttacks[64][8];
    U64 firstFileAttacks[64][8];

    /**
     * @brief This array contains for each possible position on which the king
     * might be at, the associated bitboard with all the attack positions
     * marked.
     */
    U64 kingNeighbors[64];

 private:
    Board& _board;
    Logger _logger;

    U8 generateLineAttacks(U8 rook, U8 occ);
    void initFirstRankAttacks();
    void initFirstFileAttacks();
    /**
     * @brief This functions precomputes the possible moves/attacks for a king
     * based on his current position. It is called only by the constructor.
     */
    void initKingNeighbors();

    void whitePawnMoves(uint16_t* moves, uint16_t* len);
    void blackPawnMoves(uint16_t* moves, uint16_t* len);
    void whitePawnAttacks(uint16_t* moves, uint16_t* len);
    void blackPawnAttacks(uint16_t* moves, uint16_t* len);

    U64 getRookFileAttackBB(uint16_t rookRank, uint16_t rookFile,
            U64 occ, U64 friendPieceBB);
    U64 getRookRankAttackBB(uint16_t rookRank, uint16_t rookFile,
            U64 occ, U64 friendPieceBB);
    void rookAttacks(uint16_t *moves, uint16_t *len, U64 rookBB,
            U64 friendPieceBB);
    void whiteRookAttacks(uint16_t* moves, uint16_t* len);
    void blackRookAttacks(uint16_t* moves, uint16_t* len);

    void bishopMoves(uint16_t* moves, uint16_t* len);

    void knightMoves(uint16_t* moves, uint16_t* len);

    void queenMoves(uint16_t* moves, uint16_t* len);

    /**
     * @brief This functions adds to an array the possible king moves.
     * 
     * @param side which side's king is supposed to move
     * @param moves a pointer to an array in which to add the new moves
     * @param len a pointer to the the length of the array, in the end it will
     * be updated
     */
    void kingMoves(Side side, uint16_t* moves, uint16_t* len);

    /**
     * @brief This functions adds to an array the possible king attacks.
     * 
     * @param side which side's king is supposed to move
     * @param moves a pointer to an array in which to add the new moves
     * @param len a pointer to the the length of the array, in the end it will
     * be updated
     */
    void kingAttacks(Side side, uint16_t* moves, uint16_t* len);
};
