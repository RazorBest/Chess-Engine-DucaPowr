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

    // vvvvv Perhaps theese should be private?
    U64 firstRankAttacks[64][8];
    U64 firstFileAttacks[64][8];
    U64 bishopAttackTable[64][512];
    U64 bishopMask[64];
    /* Masks for the ascending diagonals
     * Where ascDiagMask[0] is a mask for the A8-A8 diagonal
     *   and ascDiag[14]    is a mask for the H1-H1 diagonal.
     * So the first 5 diagonals are indexed like this:
     *
     * ........   
     * ........   
     * ........   
     * .......4
     * ......43
     * .....432
     * ....4321
     * ...43210
     */
    U64 ascDiagMask[15];
    /* Masks for the descending diagonals
     * Where desDiagMask[0] is a mask for the A1-A1 diagonal
     *   and ascDiag[14]    is a mask for the H8-H8 diagonal.
     * So the first 5 diagonals are indexed like this:
     *
     * ........   
     * ........   
     * ........   
     * 4.......
     * 34......
     * 234.....
     * 1234....
     * 01234...
     */
    U64 desDiagMask[15];

 private:
    Board& _board;
    Logger _logger;

    // Bitboards off all possible knight moves from square i, 0 <= i < 64
    U64 knightPosMoves[64];

    U8 generateLineAttacks(U8 rook, U8 occ);
    void initFirstRankAttacks();
    void initFirstFileAttacks();
    void initDiagMasks();
    void initBishopMask();
    void initPositionedBishopAttackTable(int bishopIndex);
    void initBishopAttackTable();

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

    void bishopAttacks(uint16_t* moves, uint16_t* len, U64 bishopBB,
            U64 friendPieceBB);
    void whiteBishopAttacks(uint16_t* moves, uint16_t* len);
    void blackBishopAttacks(uint16_t* moves, uint16_t* len);

    void knightMoves(uint16_t* moves, uint16_t* len);
    void initKnightPosMoves();

    void queenAttacks(uint16_t* moves, uint16_t* len, U64 queenBBm,
            U64 friendPieceBB);
    void blackQueenAttacks(uint16_t* moves, uint16_t* len);
    void whiteQueenAttacks(uint16_t* moves, uint16_t* len);

    void kingMoves(uint16_t* moves, uint16_t* len);
};
