/* Copyright 2021 DucaPowr Team */
#include "./moveChecker.h"
#include "./utils.h"

MoveChecker::MoveChecker(Board& board) : _board(board) { }

bool MoveChecker::isLegal(uint16_t move, uint16_t* moves, uint16_t moves_len) {
    // check if other moves have dest as our king
    uint16_t kingPos = getSquareIndex(_board.getKingBB(otherSide(_board.sideToMove)));

    for (uint16_t i = 0; i < moves_len; i++) {
        if (((moves[i] >> 6) & 0x3f) == kingPos)
            return false;
    }

    // if our move is castling check if our king gets in check
    if (((move >> 14) & 3) != 3) {
        // Castling flag was not set, nothing to do.
        return true;
    }

    uint16_t source = move & 0x3f;
    uint16_t dest = (move >> 6) & 0x3f;

    // check source
    for (uint16_t i = 0; i < moves_len; i++) {
        if (((moves[i] >> 6) & 0x3f) == source)
            return false;
    }

    if (dest > source) {
        // check source + 1 / dest - 1
        for (uint16_t i = 0; i < moves_len; i++) {
            if (((moves[i] >> 6) & 0x3f) == source + 1)
                return false;
        }
    } else {
        // check source - 1 / dest + 1
        for (uint16_t i = 0; i < moves_len; i++) {
            if (((moves[i] >> 6) & 0x3f) == source - 1)
                return false;
        }
    }

    return true;
}


// This doesn't check the correctness of the move
// bool MoveChecker::isEnPassant(uint16_t move) {
//     // Extract the last 3 bits
//     uint16_t dst = move & 0x3F;
//     // Extract the next 6 bits
//     uint16_t src = move & 0xFC0;

//     U64 dstBB = 1LL << dst;
//     U64 srcBB = 1LL << src;
//     U64 whitePawn = _board.getPawnBB(whiteSide);
//     U64 blackPawn = _board.getPawnBB(blackSide);
//     U64 allPcs = _board.getAllBB();

//     // If the move is a normal attack
//     if (dstBB & allPcs) {
//         return false;
//     }

//     // If the source is not a pawn
//     if (!(srcBB & whitePawn) && !(srcBB & blackPawn)) {
//         return false;
//     }

//     // If the pawn doesn't move one space diagonally
//     if (!(dstBB == (srcBB << 9) ||
//         dstBB == (srcBB << 7) ||
//         dstBB == (src >> 9) ||
//         dstBB == (src >> 7))) {
//         return false;
//     }

//     return true;
// }
