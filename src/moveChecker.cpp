/* Copyright 2021 DucaPowr Team */
#include "./moveChecker.h"
#include "./utils.h"

MoveChecker::MoveChecker(Board& board) : _board(board) { }

bool MoveChecker::isLegal(uint16_t move, U64 attackBB) {
    // is my king in check and i dont move it


    // if our move is castling check if our king gets in check
    if (((move >> 14) & 3) != 3) {
        // Castling flag was not set, nothing to do.
        return true;
    }

    uint16_t kingPos        = move & 0x3f;
    U64 kingBB              = 1;
    kingBB = kingBB << kingPos;

    uint16_t kingCastleDest = (move >> 6) & 0x3f;
    U64 destBB              = 1;
    destBB = destBB << kingCastleDest;

    if (kingBB & attackBB)
        return false;

    if (destBB & attackBB)
        return false;

    if (kingPos < kingCastleDest) {
        // check source + 1 / dest - 1
        if ((kingBB << 1) & attackBB)
            return false;
    } else {
        if ((kingBB >> 1) & attackBB)
            return false;
    }

    return true;
}

bool MoveChecker::isCheck(U64 attackBB) {
    U64 kingBB = _board.getKingBB(_board.sideToMove);

    return kingBB & attackBB;
}

bool MoveChecker::IamInCheck(U64 attackBB) {
    U64 kingBB = _board.getKingBB(otherSide(_board.sideToMove));

    return kingBB & attackBB;
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
