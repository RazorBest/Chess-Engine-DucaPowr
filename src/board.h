#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <string>

typedef uint64_t U64;

enum Side {
    whiteSide,
    blackSide
};

class Board  {
    private:
        // An array of piece bitboards
        U64 pieceBB[12];

    public:
        /* Side is either 0 (white) or 1 (black) */

        // Get bitboard of pieces on the coresponding side
        U64 piece(Side side);
        U64 pawn(Side side);
        U64 bishop(Side side);
        U64 knight(Side side);
        U64 rook(Side side);
        U64 queen(Side side);
        U64 king(Side side);

        U64 all();

        uint16_t convertMove(std::string move);
};
