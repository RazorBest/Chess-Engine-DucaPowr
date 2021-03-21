#include "board.h"
#include "constants.h"

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
    nBlackKing
};

void Board::init() {
    pieceBB[nWhitePawn] = WHITEPAWNSTART;                                   
    pieceBB[nBlackPawn] = BLACKPAWNSTART;                                   
    pieceBB[nWhiteBishop] = WHITEBISHOPSTART;                               
    pieceBB[nBlackBishop] = BLACKBISHOPSTART;                               
    pieceBB[nWhiteKnight] = WHITEKNIGHTSTART;                               
    pieceBB[nBlackKnight] = BLACKKNIGHTSTART;                               
    pieceBB[nWhiteRook] = WHITEROOKSTART;                                   
    pieceBB[nBlackRook] = BLACKROOKSTART;                                   
    pieceBB[nWhiteQueen] = WHITEQUEENSTART;                                 
    pieceBB[nBlackQueen] = BLACKQUEENSTART;                                 
    pieceBB[nWhiteKing] = WHITEKINGSTART;                                   
    pieceBB[nBlackKing] = BLACKKINGSTART;
}

U64 Board::piece(Side side) {
    U64 *BB = this->pieceBB;

    // Perform bitwise OR between all white pieces
    return  BB[nWhitePawn + side] | 
            BB[nWhiteBishop + side] | 
            BB[nWhiteKnight + side] | 
            BB[nWhiteRook + side] | 
            BB[nWhiteQueen + side] | 
            BB[nWhiteKing + side];
}

U64 Board::pawn(Side side) {
    return this->pieceBB[nWhitePawn + side];
}

U64 Board::bishop(Side side) {
    return this->pieceBB[nWhiteBishop + side];
}


U64 Board::knight(Side side) {
    return this->pieceBB[nWhiteKnight + side];
}

U64 Board::rook(Side side) {
    return this->pieceBB[nWhiteRook + side];
}

U64 Board::queen(Side side) {
    return this->pieceBB[nWhiteQueen + side];
}

U64 Board::king(Side side) {
    return this->pieceBB[nWhiteKing + side];
}

U64 Board::all() {
    return this->piece(whiteSide) | this->piece(blackSide);
}

int Board::getPieceIndexFromSquare(uint16_t sq) {
    // Convert from index(0-63) to bitboard 
    U64 sqBB = 1 << sq;

    // Iterate through every bitboard
    for (int i = 0; sizeof(pieceBB) / sizeof(*pieceBB); i++) {
        if (pieceBB[i] & sqBB) {
            return i;
        }  
    }

    return -1;
}

// This doesn't check the corectness of the move
bool Board::isEnPassant(uint16_t move) {
    // Extract the last 6 bits
    uint16_t dst = move & 0x3F;
    // Extract the next 6 bits
    uint16_t src = move & 0xFC0;

    U64 dstBB = 1 << dst;
    U64 srcBB = 1 << src;
    U64 whitePawn = pawn(whiteSide);
    U64 blackPawn = pawn(blackSide);
    U64 allPcs = all();

    // If the move is a normal attack
    if (dstBB & allPcs) {
        return false;
    }

    // If the source is not a pawn
    if (!(srcBB & whitePawn) && !(srcBB & blackPawn)) {
        return false;
    }

    // If the pawn doesn't move one space diagonally
    if (!(dstBB == (srcBB << 9) || 
        dstBB == (srcBB << 7) || 
        dstBB == (src >> 9) || 
        dstBB == (src >> 7))) {
        return false;
    }

    return true;
}
