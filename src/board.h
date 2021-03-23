#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <cstring>
#include <string>
#include <vector>

typedef uint64_t U64;

#define memeset memset

enum Side {
    whiteSide,
    blackSide
};

class Board  {
    private:
        // An array of piece bitboards
        U64 pieceBB[12];

        /** 
         * 
         * @param bb is a bitboard
         */
        std::vector<U64> getSeparatedBits(U64 bb);

        /** 
         * It finds the piece that is on that square and it returns the index 
         * to its bitboard in the pieceBB array
         * @param sq is a 0-63 number indicating a board square
         */
        int getPieceIndexFromSquare(uint16_t sq);

    public:
        // Init function that resets the board to initial state
        void init();
        
        /* Side is either 0 (white) or 1 (black) */

        // Get bitboard of pieces on the coresponding side
        U64 getPieceBB(Side side);
        U64 getPawnBB(Side side);
        U64 getBishopBB(Side side);
        U64 getKnightBB(Side side);
        U64 getRookBB(Side side);
        U64 getQueenBB(Side side);
        U64 getKingBB(Side side);

        U64 getAllBB();

        // SAN Move Converters
        uint16_t convertSanToMove(std::string move);
        std::string convertMoveToSan(uint16_t move);
        
        std::string toString();
};
