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
    // Pieces used for applyMove() and undoMove() optimization.
    trashPiece,
    trashPieceBlack
};

class Board  {
 private:
    Logger logger;

    // An array of piece bitboards
    U64 pieceBB[14];

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

    // Moves, as described in "./moveGen.h".
    std::stack<uint16_t> moveHistory;
    // A history of pieces taken:
    std::stack<enum enumPiece> takeHistory;
    // A history of the flags before each move:
    std::stack<U64> flagsHistory;

    /**
     * Helper function, sets all en passant-able flags of the side to move to 
     * false.
    */
    void resetEnPassant();
    /**
     * Helper function, sets the en passant-able flag of a specific pawn and 
     * side to true, if necessary.
    */
    void setEnPassant(uint16_t move);
    /**
     * Helper function, moves an en passant-able pawn back one square. 
     * This allows applyMove() to work normally with less checks.
    */
    void enPassantAttackPrep(uint16_t move);
    // Helper function for undoMove().
    void undoEnPassantAttackPrep();

    // Promote a pawn to the piece given in the move, if necessary.
    void promote(uint16_t move);
    // Helper function for undoMove(). Undo a pawn promotion, if necessary.
    void demote();

    // Move the rook to its post castle position, if needed.
    void castle(uint16_t move);
    /**
     * Helper function for undoMove(). Move the rook back to its original 
     * position, if needed.
    */
   void undoCastle();

    /**
     * Helper function for applyMove(). 
     * Sets castling rights flags to false based on the moved piece, if needed.
    */
   void resetCastleFlags(enum enumPiece movedPieceIndex, U64 srcPosBitboard);

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
    /**
     * @return Returns a bitboard of the pawns that just "jumped" (moved 2
     * squares as their first move) and can therefore be attacked by an en
     * passant move.
    */
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
};
