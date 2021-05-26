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

    U64 getAttackBB(Side side);

    // vvvvv Perhaps these should be private?
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
    // ^^^^^ Perhaps these should be private?

    /**
     * @brief This array contains for each possible position on which the king
     * might be at, the associated bitboard with all the attack positions
     * marked.
     */
    U64 kingNeighbors[64];

 private:
    Board& _board;
    Logger _logger;

    // Bitboards off all possible knight moves from square i, 0 <= i < 64
    U64 knightPosMoves[64];

    U8 generateLineAttacks(U8 rook, U8 occ);
    void initFirstRankAttacks();
    void initFirstFileAttacks();

    /**
     * @brief This functions precomputes the possible moves/attacks for a king
     * based on his current position. It is called only by the constructor.
     */
    void initKingNeighbors();
    void initDiagMasks();
    void initBishopMask();
    void initPositionedBishopAttackTable(int bishopIndex);
    void initBishopAttackTable();

    U64 getPositionedRookAttackBB(Side side, U64 rookBB);
    U64 getRookAttackBB(Side side);
    U64 getPositionedBishopAttackBB(Side side, U64 bishopBB);
    U64 getBishopAttackBB(Side side);
    U64 getKnightAttackBB(Side side);
    U64 getQueenAttackBB(Side side);
    U64 getWhitePawnAttackBB();
    U64 getBlackPawnAttackBB();

    void whitePawnMoves(uint16_t* moves, uint16_t* len);
    void blackPawnMoves(uint16_t* moves, uint16_t* len);
    void whitePawnAttacks(uint16_t* moves, uint16_t* len);
    void blackPawnAttacks(uint16_t* moves, uint16_t* len);

    U64 getRookFileAttackBB(uint16_t rookRank, uint16_t rookFile,
            U64 occ, U64 friendPieceBB);
    U64 getRookRankAttackBB(uint16_t rookRank, uint16_t rookFile,
            U64 occ, U64 friendPieceBB);
    void rookAttacks(uint16_t *moves, uint16_t *moves_len, 
        uint16_t *attacks, uint16_t *attacks_len, U64 rookBB,
        U64 friendPieceBB);
    void whiteRookAttacks(uint16_t *moves, uint16_t *len,
        uint16_t *attacks, uint16_t *attacks_len);
    void blackRookAttacks(uint16_t *moves, uint16_t *len,
        uint16_t *attacks, uint16_t *attacks_len);

    void bishopAttacks(uint16_t* moves, uint16_t* len,
            uint16_t *attacks, uint16_t *attacks_len,
            U64 bishopBB, U64 friendPieceBB);
    void whiteBishopAttacks(uint16_t* moves, uint16_t* len,
            uint16_t *attacks, uint16_t *attacks_len);
    void blackBishopAttacks(uint16_t* moves, uint16_t* len,
            uint16_t *attacks, uint16_t *attacks_len);

    void initKnightPosMoves();
    void knightMoves(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len,
        U64 knightBB, U64 friendPieceBB);
    void whiteKnightMoves(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len);
    void blackKnightMoves(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len);

    void queenAttacks(uint16_t* moves, uint16_t* len,
            uint16_t *attacks, uint16_t *attacks_len, 
            U64 queenBBm, U64 friendPieceBB);
    void blackQueenAttacks(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len);
    void whiteQueenAttacks(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len);

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

    void whiteCastle(uint16_t* moves, uint16_t* len);
    void blackCastle(uint16_t* moves, uint16_t* len);
};
