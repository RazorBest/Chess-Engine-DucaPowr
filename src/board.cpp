#include <bits/stdint-uintn.h>

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

U64 Board::getPieceBB(Side side) {
    U64 *BB = this->pieceBB;

    // Perform bitwise OR between all white pieces
    return  BB[nWhitePawn + side] | 
            BB[nWhiteBishop + side] | 
            BB[nWhiteKnight + side] | 
            BB[nWhiteRook + side] | 
            BB[nWhiteQueen + side] | 
            BB[nWhiteKing + side];
}

U64 Board::getPawnBB(Side side) {
    return pieceBB[nWhitePawn + side];
}

U64 Board::getBishopBB(Side side) {
    return pieceBB[nWhiteBishop + side];
}


U64 Board::getKnightBB(Side side) {
    return pieceBB[nWhiteKnight + side];
}

U64 Board::getRookBB(Side side) {
    return pieceBB[nWhiteRook + side];
}

U64 Board::getQueenBB(Side side) {
    return pieceBB[nWhiteQueen + side];
}

U64 Board::getKingBB(Side side) {
    return pieceBB[nWhiteKing + side];
}

U64 Board::getAllBB() {
    return getPieceBB(whiteSide) | getPieceBB(blackSide);
}

uint16_t Board::convertMove(std::string move) {
    // TODO - check special flags
    uint16_t src = ((move[1] - '1') << 3) + (move[0] - 'a');
    uint16_t dest = ((move[3] - '1') << 3) + (move[2] - 'a');
    uint8_t prom = 0;
    uint8_t flags = 0;
    if (move.size() == 5) {
        switch (move[4]) {
            case 'r':
                prom = 0;
                break;
            case 'n':
                prom = 1;
                break;
            case 'b':
                prom = 2;
                break;
            case 'q':
                prom = 3;
                break;
            default:
                break;
        }
    }
    return (flags << 14) + (prom << 12) + (src << 6) + dest;
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
    U64 whitePawn = getPawnBB(whiteSide);
    U64 blackPawn = getPawnBB(blackSide);
    U64 allPcs = getAllBB();

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
