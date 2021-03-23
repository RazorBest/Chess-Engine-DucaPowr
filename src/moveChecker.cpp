#include "moveChecker.h"

MoveChecker::MoveChecker(Board& board) {
    board = board;
}

// This doesn't check the corectness of the move
bool MoveChecker::isEnPassant(uint16_t move) {
    // Extract the last 3 bits
    uint16_t dst = move & 0x3F;
    // Extract the next 6 bits
    uint16_t src = move & 0xFC0;

    U64 dstBB = 1 << dst;
    U64 srcBB = 1 << src;
    U64 whitePawn = board.pawn(whiteSide);
    U64 blackPawn = board.pawn(blackSide);
    U64 allPcs = board.all();

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
