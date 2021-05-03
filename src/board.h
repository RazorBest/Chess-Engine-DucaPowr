/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <cstring>
#include <string>
#include <iostream>
#include <stack>

#include "./logger.h"
#include "./utils.h"

#define memeset memset

enum Side {
    whiteSide,
    blackSide
};

enum enumPiece {
    nWhitePawn,
    nBlackPawn,
    nWhiteBishop,
    nBlackBishop,
    nWhiteKnight,
    nBlackKnight,
    nWhiteRook,
    nBlackRook,
    nWhiteQueen,
    nBlackQueen,
    nWhiteKing,
    nBlackKing,
    // Piece used for applyMove() and undoMove() optimization.
    trashPiece
};

class Board  {
 private:
    Logger logger;

    // An array of piece bitboards
    U64 pieceBB[13];

    /**
     * Flag bits used for (1 means move is doable, 0 otherwise):
     * [0..7] - en passant-able pawns for white side, from file a to h
     * [8..15] - en passant-able pawns for black side, from file a to h
     * 16 - castle-able queen side for white
     * 17 - castle-able king side for white
     * 18 - castle-able queen side for black
     * 19 - castle-able king side for black
    */
    U64 flags = 0xf0000;

    /**
     * It finds the piece that is on that square and it returns the index 
     * to its bitboard in the pieceBB array
     * @param sq is a 0-63 number indicating a board square
     */
    enum enumPiece getPieceIndexFromSquare(uint16_t sq);

 public:
    // state vars
    Side sideToMove;
    void switchSide();

    // Init function that resets the board to initial state
    void init();

    // Get bitboard of pieces on the coresponding side
    /* Side is either 0 (white) or 1 (black) */
    U64 getPieceBB(Side side);
    U64 getPawnBB(Side side);
    U64 getBishopBB(Side side);
    U64 getKnightBB(Side side);
    U64 getRookBB(Side side);
    U64 getQueenBB(Side side);
    U64 getKingBB(Side side);
    U64 getEnPassantablePawnsBB(Side side);
    U64 getAllBB();
    U64 getEmptyBB();

    /**
     * Applies a move to the bitboards.
     * @return  Returns false if move is not legal.
    */
    bool applyMove(uint16_t move);
    /**
     * Undoes the last move on the bitboards.
     * @return Returns whether a move was undone or not.
    */
    bool undoMove();

    // SAN Move Converters
    uint16_t convertSanToMove(std::string move);
    std::string convertMoveToSan(uint16_t move);

    std::string toString();

private:
    // Moves, as described in "./moveGen.h".
    std::stack<uint16_t> moveHistory;
    /**
     * A history of pieces taken:
     * 0 - none, 1 - pawn, 2 - rook, 3 - knight, 4 - bishop, 5 - queen.
     * [6, 255] - unused.
    */
    std::stack<enum enumPiece> takeHistory;
};
