#include "board.h"
#include <bits/stdint-uintn.h>

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

U64 Board::piece(Side side) {
    U64 *BB = this->pieceBB;

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

uint16_t Board::convertMove(std::string move) {
    uint16_t src = ((move[1] - '1') << 3) + (move[0] - 'a');
    uint16_t dst = ((move[3] - '1') << 3) + (move[2] - 'a');
    return (src << 6) + src;
}
