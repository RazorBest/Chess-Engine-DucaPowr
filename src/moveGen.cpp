/* Copyright 2021 DucaPowr Team */
#include "./moveGen.h"

Generator::Generator(Board& board) : _board(board) {
    initFirstRankAttacks();
}

U8 generateLineAttacks(U8 rook, U8 occ) {
    // If occupant pieces overlap with rook
    if (rook & occ) {
        return 0;
    }

    return (occ - rook) ^ reverse(reverse(occ) - reverse(rook));
}

void Generator::initFirstRankAttacks() {
    // rook = the bitboard of one rook over a rank
    // occ  = the bitboard of the occupant pieces over a rank
    U8 rook, occ;

    // Iterate with the rook over the 8 squares of the rank
    for (int rook_index = 0; rook_index < 8; rook_index++) {
        rook = 1;
        rook = rook << rook_index;
        // If the occupancy rank is empty, the rook can move anywhere
        firstRankAttacks[0][rook_index] = 0xFF;
        // Generate all combinations of the occupancy rank
        for (U8 occ_index = 0; occ_index < 64; occ_index++) {
            occ = occ_index << 1;

            U8 lineAttacks = generateLineAttacks(rook, occ);
            firstRankAttacks[occ_index][rook_index] = lineAttacks;
        }
    }
}

void Generator::generateMoves(uint16_t* moves, uint16_t* len) {
    if (_board.sideToMove == whiteSide) {
        whitePawnMoves(moves, len);
        whitePawnAttacks(moves, len);
    } else {
        blackPawnMoves(moves, len);
        blackPawnAttacks(moves, len);
    }
}

void Generator::whitePawnMoves(uint16_t* moves, uint16_t *len) {
    uint64_t emptyPiece = _board.getEmptyBB();
    uint64_t possibleMoves;
    uint64_t promotions;
    uint64_t possibleMovesJump;

    possibleMoves = (_board.getPawnBB(whiteSide) << 8) & (~RANK8) & emptyPiece;
    promotions = (_board.getPawnBB(whiteSide) << 8) & RANK8 & emptyPiece;
    possibleMovesJump = (possibleMoves << 8) & RANK4 & emptyPiece;

    std::vector<U64> separated = getSeparatedBits(possibleMoves);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst >> 8);

        moves[(*len)++] = tmp;
    }

    separated = getSeparatedBits(promotions);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst >> 8);

        // Set bits for queen promotion
        tmp |= 0x3000;
        moves[(*len)++] = tmp;

        // Switch the promotion bits back to 0
        tmp ^= 0x3000;
        // Set bits for knight promotion
        tmp |= 0x1000;
        moves[(*len)++] = tmp;
    }

    separated = getSeparatedBits(possibleMovesJump);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst >> 16);

        moves[(*len)++] = tmp;
    }
}

void Generator::blackPawnMoves(uint16_t* moves, uint16_t* len) {
    uint64_t emptyPiece = _board.getEmptyBB();
    uint64_t possibleMoves;
    uint64_t promotions;
    uint64_t possibleMovesJump;

    possibleMoves = (_board.getPawnBB(blackSide) >> 8) & (~RANK1) & emptyPiece;
    promotions = (_board.getPawnBB(blackSide) >> 8) & RANK1 & emptyPiece;
    possibleMovesJump = (possibleMoves >> 8) & RANK5 & emptyPiece;

    std::vector<U64> separated = getSeparatedBits(possibleMoves);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst << 8);

        moves[(*len)++] = tmp;
    }

    separated = getSeparatedBits(promotions);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst << 8);

        // Set bits for queen promotion
        tmp |= 0x3000;
        moves[(*len)++] = tmp;

        // Set the promotion bits back to 0
        tmp ^= 0x3000;
        // Set bits for knight promotion
        tmp |= 0x1000;
        moves[(*len)++] = tmp;
     }

    separated = getSeparatedBits(possibleMovesJump);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst << 16);

        moves[(*len)++] = tmp;
    }
}

void Generator::whitePawnAttacks(uint16_t* moves, uint16_t* len) {
    U64 opponentBB = _board.getPieceBB(blackSide);
    U64 pawnBB = _board.getPawnBB(whiteSide);
    U64 leftAttacks;
    U64 rightAttacks;
    U64 leftPromotions;
    U64 rightPromotions;

    leftAttacks = (pawnBB << 7) & (~HFILE) & opponentBB & (~RANK8);
    rightAttacks = (pawnBB << 9) & (~AFILE) & opponentBB & (~RANK8);
    leftPromotions = (pawnBB << 7) & (~HFILE) & opponentBB & RANK8;
    rightPromotions = (pawnBB << 9) & (~AFILE) & opponentBB & RANK8;

    std::vector<U64> separated;

    // Generate a move for every left attack
    separated = getSeparatedBits(leftAttacks);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst >> 7);

        moves[(*len)++] = tmp;
    }

    // Generate a move for every right attack
    separated = getSeparatedBits(rightAttacks);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst >> 9);

        moves[(*len)++] = tmp;
    }

    // Generate a move for every left attack -> promotion
    separated = getSeparatedBits(leftPromotions);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst >> 7);

        // Set bits for queen promotion
        tmp |= 0x3000;
        moves[(*len)++] = tmp;

        // Set the promotion bits back to 0
        tmp ^= 0x3000;
        // Set bits for knight promotion
        tmp |= 0x1000;
        moves[(*len)++] = tmp;
    }

    // Generate a move for every right attack -> promotion
    separated = getSeparatedBits(rightPromotions);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst >> 9);

        // Set bits for queen promotion
        tmp |= 0x3000;
        moves[(*len)++] = tmp;

        // Set the promotion bits back to 0
        tmp ^= 0x3000;
        // Set bits for knight promotion
        tmp |= 0x1000;
        moves[(*len)++] = tmp;
    }
}

void Generator::blackPawnAttacks(uint16_t* moves, uint16_t* len) {
    U64 opponentBB = _board.getPieceBB(whiteSide);
    U64 pawnBB = _board.getPawnBB(blackSide);
    U64 leftAttacks;
    U64 rightAttacks;
    U64 leftPromotions;
    U64 rightPromotions;

    leftAttacks = (pawnBB >> 9) & (~HFILE) & opponentBB & (~RANK1);
    rightAttacks = (pawnBB >> 7) & (~AFILE) & opponentBB & (~RANK1);
    leftPromotions = (pawnBB >> 9) & (~HFILE) & opponentBB & RANK1;
    rightPromotions = (pawnBB >> 7) & (~AFILE) & opponentBB & RANK1;

    std::vector<U64> separated;

    // Generate a move for every left attack
    separated = getSeparatedBits(leftAttacks);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst << 9);

        moves[(*len)++] = tmp;
    }

    // Generate a move for every right attack
    separated = getSeparatedBits(rightAttacks);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst << 7);

        moves[(*len)++] = tmp;
    }

    // Generate a move for every left attack -> promotion
    separated = getSeparatedBits(leftPromotions);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst << 9);

        // Set bits for queen promotion
        tmp |= 0x3000;
        moves[(*len)++] = tmp;

        // Set the promotion bits back to 0
        tmp ^= 0x3000;
        // Set bits for knight promotion
        tmp |= 0x1000;
        moves[(*len)++] = tmp;
    }

    // Generate a move for every right attack -> promotion
    separated = getSeparatedBits(rightPromotions);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst << 7);

        // Set bits for queen promotion
        tmp |= 0x3000;
        moves[(*len)++] = tmp;

        // Set the promotion bits back to 0
        tmp ^= 0x3000;
        // Set bits for knight promotion
        tmp |= 0x1000;
        moves[(*len)++] = tmp;
    }
}
