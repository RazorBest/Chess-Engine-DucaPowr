/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <cstring>
#include <string>
#include <iostream>

#include "./logger.h"
#include "./utils.h"

#define memeset memset

enum Side {
    whiteSide,
    blackSide
};

class Board  {
 private:
    Logger logger;

    // An array of piece bitboards
    U64 pieceBB[13];

    /**
     * It finds the piece that is on that square and it returns the index
     * to its bitboard in the pieceBB array
     * @param sq is a 0-63 number indicating a board square
     */
    int getPieceIndexFromSquare(uint16_t sq);

 public:
    // state vars
    Side sideToMove;
    void switchSide();

    // Init function that resets the board to initial state
    void init();

    // Get bitboard of pieces on the corresponding side
    /* Side is either 0 (white) or 1 (black) */
    U64 getPieceBB(Side side);
    U64 getPawnBB(Side side);
    U64 getBishopBB(Side side);
    U64 getKnightBB(Side side);
    U64 getRookBB(Side side);
    U64 getQueenBB(Side side);
    U64 getKingBB(Side side);
    U64 getAllBB();
    U64 getEmptyBB();

    // apply move - returns false if move is not legal
    bool applyMove(uint16_t move);

    // SAN Move Converters
    uint16_t convertSanToMove(std::string move);
    std::string convertMoveToSan(uint16_t move);

    std::string toString();
};
