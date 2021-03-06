/* Copyright 2021 DucaPowr Team */
#include "./moveGen.h"
#include "board.h"
#include "constants.h"
#include "./utils.h"
#include "logger.h"
#include <bits/stdint-uintn.h>
#include <cstdio>
#include <string>
#include <vector>

Generator::Generator(Board& board) : _board(board) {
    initFirstRankAttacks();
    initFirstFileAttacks();
    initKingNeighbors();
    initDiagMasks();
    initBishopMask();
    initBishopAttackTable();
    initKnightPosMoves();

    if (DEBUG) {
        _logger.info("Finished initialising the Generator");
    }
}

/**
 * Note: any piece can be "moved" by one square doing (pos is the bitboard of its position, 1 bit):
 *           (pos << 8)
 *               ^
 *               |
 * (pos >> 1) <- P -> (pos << 1)
 *               |
 *               v
 *           (pos >> 8)
*/

U8 Generator::generateLineAttacks(U8 rook, U8 occ) {
    // If occupant pieces overlap with rook
    if (rook & occ) {
        return 0;
    }

    return (occ - rook) ^ reverse(reverse(occ) - reverse(rook));
}

void Generator::initFirstRankAttacks(void) {
    // rook = the bitboard of one rook over a rank
    // occ  = the bitboard of the occupant pieces over a rank
    U8 rook, occ;

    // Iterate with the rook over the 8 squares of the rank
    for (int rook_index = 0; rook_index < 8; rook_index++) {
        rook = 1;
        rook = rook << rook_index;
        // Generate all combinations of the occupancy rank
        for (U8 occ_index = 0; occ_index < 64; occ_index++) {
            occ = occ_index << 1;

            U8 lineAttacks = generateLineAttacks(rook, occ);
            firstRankAttacks[occ_index][rook_index] = lineAttacks;
        }
    }
}

void Generator::initFirstFileAttacks(void) {
    // rook = the bitboard of one rook
    // occ  = the bitboard of the occupant pieces
    U64 rook, occ;

    // Iterate with the rook over the 8 squares of the rank
    for (int rook_index = 0; rook_index < 8; rook_index++) {
        rook = 1;
        rook = rook << rook_index;
        // Generate all combinations of the occupancy rank
        for (U8 occ_index = 0; occ_index < 64; occ_index++) {
            occ = occ_index << 1;

            U64 lineAttacks = generateLineAttacks(rook, occ);
            // Move to the 8th rank
            lineAttacks <<= 56;
            U64 fileAttacks = rotate90AntiClockwise(lineAttacks);
            firstFileAttacks[occ_index][rook_index] = fileAttacks;
        }
    }
}


void Generator::initKingNeighbors(void) {
    U64 kingBB = 1;
    for (size_t i = 0; i < 64; i++) {
        kingNeighbors[i] = aKingsNeighbors(kingBB);
        kingBB <<= 1;
    }
}

static U64 getAscendingDiagonalMask(int rankIndex, int fileIndex)  {
    U64 mask = 1;
    mask <<= rankIndex * 8 + fileIndex;

    // Generate the bits in positive direction
    int file = fileIndex + 1;
    int rank = rankIndex + 1;
    while (file < 8 && rank < 8) {
        U64 bit = 1;
        bit <<= (rank * 8 + file);
        mask |= bit;
        rank++; file++;
    }

    // Generate the bits in negative direction
    file = fileIndex - 1;
    rank = rankIndex - 1;
    while (file >= 0 && rank >= 0) {
        U64 bit = 1;
        bit <<= (rank * 8 + file);
        mask |= bit;
        rank--; file--;
    }

    return mask;
}

static U64 getDescendingDiagonalMask(int rankIndex, int fileIndex) {
    U64 mask = 1;
    mask <<= rankIndex * 8 + fileIndex;

    // Generate the bits in positive direction
    int file = fileIndex + 1;
    int rank = rankIndex - 1;
    while (file < 8 && rank >= 0) {
        U64 bit = 1;
        bit <<= (rank * 8 + file);
        mask |= bit;
        file++; rank--;
    }

    // Generate the bits in negative direction
    file = fileIndex - 1;
    rank = rankIndex + 1;
    while (file >= 0 && rank < 8) {
        U64 bit = 1;
        bit <<= (rank * 8 + file);
        mask |= bit;
        file--; rank++;
    }

    return mask;
}

void Generator::initDiagMasks(void) {
    int index = 0;

    for (int rankIndex = 0; rankIndex < 8; rankIndex++) {
        ascDiagMask[index] = getAscendingDiagonalMask(rankIndex, 7);
        desDiagMask[index] = getDescendingDiagonalMask(rankIndex, 0);
        index++;
    }
    for (int rankIndex = 1; rankIndex < 8; rankIndex++) {
        ascDiagMask[index] = getAscendingDiagonalMask(rankIndex, 0);
        desDiagMask[index] = getDescendingDiagonalMask(rankIndex, 7);
        index++;
    }
}

void Generator::initBishopMask(void) {
    // Use this mask to ignore the margins
    // We won't need them when generating the bishop attacks
    const U64 marginMask = 0xFF818181818181FF;
    for (int sqIndex = 0; sqIndex < 64; sqIndex++) {
        int rankIndex = sqIndex / 8;
        int fileIndex = sqIndex % 8;

        // Puts the first diagonal
        bishopMask[sqIndex] = ascDiagMask[rankIndex - fileIndex + 7] & (~marginMask);
        // Puts the second diagonal and excludes the bishop square
        bishopMask[sqIndex] ^= desDiagMask[rankIndex + fileIndex] & (~marginMask);
    }
}

/* Transforms this bitboard:
 *      00000000
 *      00000000
 *      00000000
 *      00000000
 *      00000000
 *      00000000
 *      00000000
 *      12345678
 * To this:
 *      12345678
 *      12345678
 *      12345678
 *      12345678
 *      12345678
 *      12345678
 *      12345678
 *      12345678
 */
U64 mapRank1ToDiag(U64 bb) {
    // A magic that maps Rank 1 to the A1-H8 diagonal
    const U64 magicR1D = 0x0101010101010101;

    bb *= magicR1D;

    return bb;
}

void Generator::initPositionedBishopAttackTable(int sqIndex) {
    const U64 marginMask = 0xFF818181818181FF;
    int rankIndex = sqIndex / 8;
    int fileIndex = sqIndex % 8;

    U8 bishop = 1 << fileIndex;
    U64 occBB;

    // Generate every possible combination of occupants for the
    //  ascending diagonal
    for (int occ1 = 0; occ1 < 64; occ1++) {
        U8 occ = occ1 << 1;

        // If the bishop overlaps with an occupant piece
        if (occ & bishop) {
            continue;
        }

        U64 firstAttackBB;
        // Generate the attacks as if the pieces were on the first rank
        firstAttackBB = generateLineAttacks(bishop, occ);
        // Map the rank attacks to all the diagonals
        firstAttackBB = mapRank1ToDiag(firstAttackBB);
        // Mask with the diagonal that we're interested in
        firstAttackBB &= ascDiagMask[rankIndex - fileIndex + 7];

        // Map the rank with the occupants bits to all the diagonals
        U64 firstDiagBB = mapRank1ToDiag(occ);
        // Mask with the diagonal that we're interested in
        firstDiagBB &= ascDiagMask[rankIndex - fileIndex + 7];

        occBB = firstDiagBB;

        // Generate every possible combination of occupants for the
        //  descending diagonal
        for (int occ2 = 0; occ2 < 64; occ2++) {
            occ = occ2 << 1;
            // If the bishop overlaps with an occupant piece
            if (occ & bishop) {
                continue;
            }

            U64 secondAttackBB;
            // Generate the attacks as if the pieces were on the first rank
            secondAttackBB = generateLineAttacks(bishop, occ);
            // Map the rank attacks to all the diagonals
            secondAttackBB = mapRank1ToDiag(secondAttackBB);
            // Mask with the diagonal that we're interested in
            secondAttackBB &= desDiagMask[rankIndex + fileIndex];

            // Map the rank with the occupants bits to all the diagonals
            U64 secondDiagBB = mapRank1ToDiag(occ);
            // Mask with the diagonal that we're interested in
            secondDiagBB &= desDiagMask[rankIndex + fileIndex];

            U64 attackBB = firstAttackBB | secondAttackBB;

            occBB = firstDiagBB | secondDiagBB;
            // Eliminate the margins from the occupancy bitboard
            occBB &= ~marginMask;

            // Index the occupancy bitboard
            U64 indexableBB = (occBB * bishopMagics[sqIndex])
                >> (64 - bishopRelevantBits[sqIndex]);
            uint16_t occIndex = indexableBB & ((1 << bishopRelevantBits[sqIndex]) - 1);

            bishopAttackTable[sqIndex][occIndex] = attackBB;
        }
    }
}

void Generator::initBishopAttackTable(void) {
    for (int sqIndex = 0; sqIndex < 64; sqIndex++) {
        initPositionedBishopAttackTable(sqIndex);
    }
}

U64 Generator::getPositionedRookAttackBB(Side side, U64 rookBB) {
    U64 friendPieceBB = _board.getPieceBB(side);
    U64 allAttacksBB = 0;
    U64 allBB = _board.getAllBB();
    U64 occ;

    std::vector<U64> separated = getSeparatedBits(rookBB);
    for (auto piece : separated) {
        uint16_t move = getSquareIndex(piece);
        uint16_t rank = move / 8;
        uint16_t file = move % 8;

        occ = allBB & (~piece);
        U64 attacksBB = getRookRankAttackBB(rank, file, occ, friendPieceBB);

        occ = allBB & (~piece);
        attacksBB |= getRookFileAttackBB(rank, file, occ, friendPieceBB);

        allAttacksBB |= attacksBB;
     }

    return allAttacksBB;
}

U64 Generator::getRookAttackBB(Side side) {
    U64 rookBB = _board.getRookBB(side);
    return getPositionedRookAttackBB(side, rookBB);
}

U64 Generator::getPositionedBishopAttackBB(Side side, U64 bishopBB) {
    U64 friendPieceBB = _board.getPieceBB(side);
    U64 allBB = _board.getAllBB();
    U64 occ, allAttacksBB = 0, attacksBB;

    std::vector<U64> separated = getSeparatedBits(bishopBB);
    for (auto piece : separated) {
        uint16_t move = getSquareIndex(piece);

        // Index the occupancy bitboard
        occ = bishopMask[move] & allBB;
        occ = (occ * bishopMagics[move]) >> (64 - bishopRelevantBits[move]);
        uint16_t occIndex = occ & ((1 << bishopRelevantBits[move]) - 1);

        // Get the attacks bitboard
        attacksBB = bishopAttackTable[move][occIndex];
        attacksBB &= ~friendPieceBB;

        allAttacksBB |= attacksBB;
    }

    return allAttacksBB;
}

U64 Generator::getBishopAttackBB(Side side) {
    U64 bishopBB = _board.getBishopBB(side);
    return getPositionedBishopAttackBB(side, bishopBB);
}

U64 Generator::getKnightAttackBB(Side side) {
    U64 knightBB = _board.getKnightBB(side);
    U64 friendPieceBB = _board.getPieceBB(side);
    uint16_t knightIndex;
    std::vector<U64> separatedMoves;
    U64 allAttacksBB = 0;

    // All possible moves of a given knight.
    U64 allPosMoves;

    while (knightBB) {
        // Get knight from bit board.
        knightIndex = getSquareIndex(knightBB);

        // Set source of moves this knight can do.

        allPosMoves = knightPosMoves[knightIndex] & (~friendPieceBB);

        allAttacksBB |= allPosMoves;

        // Remove knight from bitboard.
        U64 one = 1;
        knightBB ^= (one << knightIndex);
    }

    return allAttacksBB;
}

U64 Generator::getQueenAttackBB(Side side) {
    U64 queenBB = _board.getQueenBB(side);
    return getPositionedRookAttackBB(side, queenBB) |
        getPositionedBishopAttackBB(side, queenBB);
}

U64 Generator::getWhitePawnAttackBB() {
    U64 opponentBB = _board.getPieceBB(blackSide);
    U64 enPassantablePawns = _board.getEnPassantablePawnsBB(blackSide);
    U64 pawnBB = _board.getPawnBB(whiteSide);
    U64 leftAttacks;
    U64 rightAttacks;
    U64 leftPromotions;
    U64 rightPromotions;
    U64 leftEnPassant;
    U64 rightEnPassant;
    U64 allAttacksBB = 0;

    leftAttacks = (pawnBB << 7) & (~HFILE) & opponentBB & (~RANK8);
    rightAttacks = (pawnBB << 9) & (~AFILE) & opponentBB & (~RANK8);
    leftPromotions = (pawnBB << 7) & (~HFILE) & opponentBB & RANK8;
    rightPromotions = (pawnBB << 9) & (~AFILE) & opponentBB & RANK8;
    leftEnPassant = (pawnBB >> 1) & (~HFILE) & enPassantablePawns;
    leftEnPassant <<= 8;
    rightEnPassant = (pawnBB << 1) & (~AFILE) & enPassantablePawns;
    rightEnPassant <<= 8;


    allAttacksBB |= leftAttacks;
    allAttacksBB |= rightAttacks;
    allAttacksBB |= leftPromotions;
    allAttacksBB |= rightPromotions;
    allAttacksBB |= leftEnPassant;
    allAttacksBB |= rightEnPassant;

    return allAttacksBB;
}

U64 Generator::getBlackPawnAttackBB() {
    U64 opponentBB = _board.getPieceBB(whiteSide);
    U64 enPassantablePawns = _board.getEnPassantablePawnsBB(whiteSide);
    U64 pawnBB = _board.getPawnBB(blackSide);
    U64 leftAttacks;
    U64 rightAttacks;
    U64 leftPromotions;
    U64 rightPromotions;
    U64 leftEnPassant;
    U64 rightEnPassant;
    U64 allAttacksBB = 0;

    leftAttacks = (pawnBB >> 9) & (~HFILE) & opponentBB & (~RANK1);
    rightAttacks = (pawnBB >> 7) & (~AFILE) & opponentBB & (~RANK1);
    leftPromotions = (pawnBB >> 9) & (~HFILE) & opponentBB & RANK1;
    rightPromotions = (pawnBB >> 7) & (~AFILE) & opponentBB & RANK1;
    leftEnPassant = (pawnBB >> 1) & (~HFILE) & enPassantablePawns;
    leftEnPassant >>= 8;
    rightEnPassant = (pawnBB << 1) & (~AFILE) & enPassantablePawns;
    rightEnPassant >>= 8;

    allAttacksBB |= leftAttacks;
    allAttacksBB |= rightAttacks;
    allAttacksBB |= leftPromotions;
    allAttacksBB |= rightPromotions;
    allAttacksBB |= leftEnPassant;
    allAttacksBB |= rightAttacks;
    allAttacksBB |= rightEnPassant;

    return allAttacksBB;
}

U64 Generator::getAttackBB(Side side) {
    U64 attackBB = 0;

    attackBB |= getRookAttackBB(side);
    attackBB |= getBishopAttackBB(side);
    attackBB |= getKnightAttackBB(side);
    attackBB |= getQueenAttackBB(side);

    if (side == whiteSide) {
        attackBB |= getWhitePawnAttackBB();
    } else {
        attackBB |= getBlackPawnAttackBB();
    }

    return attackBB;
}

void Generator::generateMoves(uint16_t* attacks, uint16_t* attacksLen) {
    uint16_t moves[MAX_MOVES_AT_STEP];
    uint16_t movesLen = 0;

    if (_board.sideToMove == whiteSide) {
        whiteCastle(moves, &movesLen);
        whiteRookAttacks(moves, &movesLen, attacks, attacksLen);
        whiteKnightMoves(moves, &movesLen, attacks, attacksLen);
        whiteBishopAttacks(moves, &movesLen, attacks, attacksLen);
        whiteQueenAttacks(moves, &movesLen, attacks, attacksLen);
        whitePawnMoves(moves, &movesLen);
        whitePawnAttacks(attacks, attacksLen);
    } else {
        blackCastle(moves, &movesLen);
        blackRookAttacks(moves, &movesLen, attacks, attacksLen);
        blackKnightMoves(moves, &movesLen, attacks, attacksLen);
        blackBishopAttacks(moves, &movesLen, attacks, attacksLen);
        blackQueenAttacks(moves, &movesLen, attacks, attacksLen);
        blackPawnMoves(moves, &movesLen);
        blackPawnAttacks(attacks, attacksLen);
    }
    kingMoves(_board.sideToMove, moves, &movesLen);
    kingAttacks(_board.sideToMove, moves, &movesLen);

     memcpy(attacks + *attacksLen, moves, movesLen * sizeof(uint16_t));
    *attacksLen += movesLen;
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

        // Set promotion flag
        tmp |= 0x4000;

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
        // Set "en passant-able" flag.
        tmp |= 0x8000;

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

        // Set promotion flag
        tmp |= 0x4000;

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
        // Set "en passant-able" flag.
        tmp |= 0x8000;

        moves[(*len)++] = tmp;
    }
}

void Generator::whitePawnAttacks(uint16_t* moves, uint16_t* len) {
    U64 opponentBB = _board.getPieceBB(blackSide);
    U64 enPassantablePawns = _board.getEnPassantablePawnsBB(blackSide);
    U64 pawnBB = _board.getPawnBB(whiteSide);
    U64 leftAttacks;
    U64 rightAttacks;
    U64 leftPromotions;
    U64 rightPromotions;
    U64 leftEnPassant;
    U64 rightEnPassant;

    leftAttacks = (pawnBB << 7) & (~HFILE) & opponentBB & (~RANK8);
    rightAttacks = (pawnBB << 9) & (~AFILE) & opponentBB & (~RANK8);
    leftPromotions = (pawnBB << 7) & (~HFILE) & opponentBB & RANK8;
    rightPromotions = (pawnBB << 9) & (~AFILE) & opponentBB & RANK8;
    leftEnPassant = (pawnBB >> 1) & (~HFILE) & enPassantablePawns;
    leftEnPassant <<= 8;
    rightEnPassant = (pawnBB << 1) & (~AFILE) & enPassantablePawns;
    rightEnPassant <<= 8;

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

        // Set promotion flag
        tmp |= 0x4000;

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

        // Set promotion flag
        tmp |= 0x4000;

        // Set bits for queen promotion
        tmp |= 0x3000;
        moves[(*len)++] = tmp;

        // Set the promotion bits back to 0
        tmp ^= 0x3000;
        // Set bits for knight promotion
        tmp |= 0x1000;
        moves[(*len)++] = tmp;
    }

    // Generate a move for every left en passant
    separated = getSeparatedBits(leftEnPassant);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst >> 7);

        moves[(*len)++] = tmp;
    }

    // Generate a move for every right en passant
    separated = getSeparatedBits(rightEnPassant);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst >> 9);

        moves[(*len)++] = tmp;
    }
}

void Generator::blackPawnAttacks(uint16_t* moves, uint16_t* len) {
    U64 opponentBB = _board.getPieceBB(whiteSide);
    U64 enPassantablePawns = _board.getEnPassantablePawnsBB(whiteSide);
    U64 pawnBB = _board.getPawnBB(blackSide);
    U64 leftAttacks;
    U64 rightAttacks;
    U64 leftPromotions;
    U64 rightPromotions;
    U64 leftEnPassant;
    U64 rightEnPassant;

    leftAttacks = (pawnBB >> 9) & (~HFILE) & opponentBB & (~RANK1);
    rightAttacks = (pawnBB >> 7) & (~AFILE) & opponentBB & (~RANK1);
    leftPromotions = (pawnBB >> 9) & (~HFILE) & opponentBB & RANK1;
    rightPromotions = (pawnBB >> 7) & (~AFILE) & opponentBB & RANK1;
    leftEnPassant = (pawnBB >> 1) & (~HFILE) & enPassantablePawns;
    leftEnPassant >>= 8;
    rightEnPassant = (pawnBB << 1) & (~AFILE) & enPassantablePawns;
    rightEnPassant >>= 8;

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

        // Set promotion flag
        tmp |= 0x4000;

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

        // Set promotion flag
        tmp |= 0x4000;

        // Set bits for queen promotion
        tmp |= 0x3000;
        moves[(*len)++] = tmp;

        // Set the promotion bits back to 0
        tmp ^= 0x3000;
        // Set bits for knight promotion
        tmp |= 0x1000;
        moves[(*len)++] = tmp;
    }

    // Generate a move for every left en passant
    separated = getSeparatedBits(leftEnPassant);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst << 9);

        moves[(*len)++] = tmp;
    }

    // Generate a move for every right en passant
    separated = getSeparatedBits(rightEnPassant);
    for (U64 attackDst : separated) {
        uint16_t tmp = getSquareIndex(attackDst);
        tmp <<= 6;
        tmp |= getSquareIndex(attackDst << 7);

        moves[(*len)++] = tmp;
    }
}

U64 Generator::getRookRankAttackBB(uint16_t rookRank, uint16_t rookFile,
        U64 occ, U64 friendPieceBB) {
    U64 rankAttacks;

    // Move the occupant pieces to the first rank
    occ = occ >> (rookRank * 8);
    U8 occ_index = occ;
    occ_index >>= 1;
    occ_index &= ~0x40;

    rankAttacks = firstRankAttacks[occ_index][rookFile];
    rankAttacks <<= (rookRank * 8);
    rankAttacks &= ~friendPieceBB;

    return rankAttacks;
}

U64 Generator::getRookFileAttackBB(uint16_t rookRank, uint16_t rookFile,
        U64 occ, U64 friendPieceBB) {
    U64 fileAttacks;

    // Map the occupancy bitboard to an occupancy index
    // occ -> occ_index

    // Move the occupant pieces to the first file
    occ = occ >> rookFile;
    // Map File A to Rank 8 in the occupancy bitboard
    occ &= AFILE;
    occ *= MAGICFAR8;

    occ >>= 57;
    U8 occ_index = occ;
    occ_index &= ~0x40;

    fileAttacks = firstFileAttacks[occ_index][rookRank];
    fileAttacks <<= rookFile;
    fileAttacks &= ~friendPieceBB;

    return fileAttacks;
}

void Generator::rookAttacks(uint16_t *moves, uint16_t *moves_len, 
        uint16_t *attacks, uint16_t *attacks_len, U64 rookBB,
        U64 friendPieceBB) {
    U64 allBB = _board.getAllBB();
    U64 occ;

    std::vector<U64> separated = getSeparatedBits(rookBB);
    for (auto piece : separated) {
        uint16_t move = getSquareIndex(piece);
        uint16_t rank = move / 8;
        uint16_t file = move % 8;

        occ = allBB & (~piece);
        U64 attacksBB = getRookRankAttackBB(rank, file, occ, friendPieceBB);

        occ = allBB & (~piece);
        attacksBB |= getRookFileAttackBB(rank, file, occ, friendPieceBB);

        // Separate quiet moves from attacks
        U64 movesBB = attacksBB & (~allBB);
        attacksBB &= allBB;

        // Iterate through all quiet moves
        std::vector<U64> separated = getSeparatedBits(movesBB);
        for (auto atk : separated) {
            uint16_t atkIndex = getSquareIndex(atk);
            move &= ~(0xFC0);
            move |= atkIndex << 6;
            moves[*moves_len] = move;
            (*moves_len)++;
        }

        // Iterate through all attacks
        separated = getSeparatedBits(attacksBB);
        for (auto atk : separated) {
            uint16_t atkIndex = getSquareIndex(atk);
            move &= ~(0xFC0);
            move |= atkIndex << 6;
            attacks[*attacks_len] = move;
            (*attacks_len)++;
        }
     }
}

void Generator::whiteRookAttacks(uint16_t *moves, uint16_t *len,
        uint16_t *attacks, uint16_t *attacks_len) {
    U64 rookBB = _board.getRookBB(whiteSide);
    U64 friendPieceBB = _board.getPieceBB(whiteSide);

    rookAttacks(moves, len, attacks, attacks_len, rookBB, friendPieceBB);
}

void Generator::blackRookAttacks(uint16_t *moves, uint16_t *len,
        uint16_t *attacks, uint16_t *attacks_len) {
    U64 rookBB = _board.getRookBB(blackSide);
    U64 friendPieceBB = _board.getPieceBB(blackSide);

    rookAttacks(moves, len, attacks, attacks_len, rookBB, friendPieceBB);
}


void Generator::kingMoves(Side side, uint16_t *moves, uint16_t *len) {
    U64 kingBB = _board.getKingBB(side);
    U64 emptyPiece = _board.getEmptyBB();
    U64 possibleMoves;
    uint16_t kingSquareIndex = getSquareIndex(kingBB);

    possibleMoves = kingNeighbors[kingSquareIndex] & emptyPiece;
    std::vector<U64> separated = getSeparatedBits(possibleMoves);
    for (auto dst : separated) {
      uint16_t tmp = 0;
      tmp = getSquareIndex(dst);
      tmp = tmp << 6;
      tmp |= getSquareIndex(kingBB);

      moves[(*len)++] = tmp;
    }
}

void Generator::kingAttacks(Side side, uint16_t *moves, uint16_t *len) {
    U64 kingBB = _board.getKingBB(side);
    U64 opponentBB = _board.getPieceBB(otherSide(side));
    U64 possibleMoves;
    uint16_t kingSquareIndex = getSquareIndex(kingBB);

    possibleMoves = kingNeighbors[kingSquareIndex] & opponentBB;
    std::vector<U64> separated = getSeparatedBits(possibleMoves);
    for (auto dst : separated) {
      uint16_t tmp = 0;
      tmp = getSquareIndex(dst);
      tmp = tmp << 6;
      tmp |= getSquareIndex(kingBB);

      moves[(*len)++] = tmp;
    }
}

void Generator::bishopAttacks(uint16_t *moves, uint16_t *len,
        uint16_t *attacks, uint16_t *attacks_len,
        U64 bishopBB, U64 friendPieceBB) {
    U64 allBB = _board.getAllBB();
    U64 occ, attacksBB;

    std::vector<U64> separated = getSeparatedBits(bishopBB);
    for (auto piece : separated) {
        uint16_t move = getSquareIndex(piece);

        // Index the occupancy bitboard
        occ = bishopMask[move] & allBB;
        occ = (occ * bishopMagics[move]) >> (64 - bishopRelevantBits[move]);
        uint16_t occIndex = occ & ((1 << bishopRelevantBits[move]) - 1);

        // Get the attacks bitboard
        attacksBB = bishopAttackTable[move][occIndex];
        attacksBB &= ~friendPieceBB;

        U64 movesBB = attacksBB & (~allBB);
        attacksBB &= allBB;

        // Iterate through all quiet moves
        std::vector<U64> separated = getSeparatedBits(movesBB);
        for (auto atk : separated) {
            uint16_t atkIndex = getSquareIndex(atk);
            move &= ~(0xFC0);
            move |= atkIndex << 6;
            moves[*len] = move;
            (*len)++;
        }

        // Iterate through all attacks
        separated = getSeparatedBits(attacksBB);
        for (auto atk : separated) {
            uint16_t atkIndex = getSquareIndex(atk);
            move &= ~(0xFC0);
            move |= atkIndex << 6;
            attacks[*attacks_len] = move;
            (*attacks_len)++;
        }
    }
}

void Generator::whiteBishopAttacks(uint16_t *moves, uint16_t *len,
        uint16_t *attacks, uint16_t *attacks_len) {
    U64 bishopBB = _board.getBishopBB(whiteSide);
    U64 friendPieceBB = _board.getPieceBB(whiteSide);

    bishopAttacks(moves, len, attacks, attacks_len, bishopBB, friendPieceBB);
}

void Generator::blackBishopAttacks(uint16_t *moves, uint16_t *len,
        uint16_t *attacks, uint16_t *attacks_len) {
    U64 bishopBB = _board.getBishopBB(blackSide);
    U64 friendPieceBB = _board.getPieceBB(blackSide);

    bishopAttacks(moves, len, attacks, attacks_len, bishopBB, friendPieceBB);
}

void Generator::queenAttacks(uint16_t* moves, uint16_t* len, 
        uint16_t *attacks, uint16_t *attacks_len,
        U64 queenBB, U64 friendPieceBB) {
    bishopAttacks(moves, len, attacks, attacks_len, queenBB, friendPieceBB);
    rookAttacks(moves, len, attacks, attacks_len, queenBB, friendPieceBB);
}
void Generator::blackQueenAttacks(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len) {
    U64 queenBB = _board.getQueenBB(blackSide);
    U64 friendPieceBB = _board.getPieceBB(blackSide);

    queenAttacks(moves, len, attacks, attacks_len, queenBB, friendPieceBB);
}
void Generator::whiteQueenAttacks(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len) {
    U64 queenBB = _board.getQueenBB(whiteSide);
    U64 friendPieceBB = _board.getPieceBB(whiteSide);

    queenAttacks(moves, len, attacks, attacks_len, queenBB, friendPieceBB);
}

void Generator::initKnightPosMoves(void) {
    int i, j;

    /* The bitboard of all the moves a knight placed at C6 can do. A chosen
    default position. */
    U64 defMoves = 0xa1100110a000000;
    const short int defFile = 2; // file C
    const short int defRank = 5; // rank 6

    // Masks that have all bits 1 save for the extreme 2 files.
    U64 leftBound = 0x3f3f3f3f3f3f3f3f;
    U64 rightBound = 0xfcfcfcfcfcfcfcfc;

   /**
    * Note: I've divided the board into 6 sectors, as follows:
    * 11222233
    * 11222233
    * 11222233
    * 44555566
    * 44555566
    * 44555566
    * 44555566
    * 44555566
    * These sectors need different masks and shifts to be obtained from the
    * default position.
   */

    // Generate all moves from sector 1.
    for (i = 0; i <= 2; i++) {
        for (j = 1; j <= 2; j++) {
            knightPosMoves[((defRank + i) << 3) + defFile - j] =
                ((defMoves >> j) << (i << 3)) & leftBound;
        }
    }

    // Generate all moves from sector 2.
    for (i = 0; i <= 2; i++) {
        for (j = 0; j <= 3; j++) {
            knightPosMoves[((defRank + i) << 3) + defFile + j] =
                (defMoves << j) << (i << 3);
        }
    }

    // Generate all moves from sector 3.
    for (i = 0; i <= 2; i++) {
        for (j = 4; j <= 5; j++) {
            knightPosMoves[((defRank + i) << 3) + defFile + j] =
                ((defMoves << j) << (i << 3)) & rightBound;
        }
    }

    // Generate all moves from sector 4.
    for (i = 1; i <= 5; i++) {
        for (j = 1; j <= 2; j++) {
            knightPosMoves[((defRank - i) << 3) + defFile - j] =
                ((defMoves >> j) >> (i << 3)) & leftBound;
        }
    }

    // Generate all moves from sector 5.
    for (i = 1; i <= 5; i++) {
        for (j = 0; j <= 3; j++) {
            knightPosMoves[((defRank - i) << 3) + defFile + j] =
                (defMoves << j) >> (i << 3);
        }
    }

    // Generate all moves from sector 6.
    for (i = 1; i <= 5; i++) {
        for (j = 4; j <= 5; j++) {
            knightPosMoves[((defRank - i) << 3) + defFile + j] =
                ((defMoves << j) >> (i << 3)) & rightBound;
        }
    }
}

void Generator::whiteKnightMoves(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len) {
    U64 knightBB = _board.getKnightBB(whiteSide);
    U64 friendPieceBB = _board.getPieceBB(whiteSide);

    knightMoves(moves, len, attacks, attacks_len, knightBB, friendPieceBB);
}

void Generator::blackKnightMoves(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len) {
    U64 knightBB = _board.getKnightBB(blackSide);
    U64 friendPieceBB = _board.getPieceBB(blackSide);

    knightMoves(moves, len, attacks, attacks_len, knightBB, friendPieceBB);
}

void Generator::knightMoves(uint16_t* moves, uint16_t* len,
        uint16_t *attacks, uint16_t *attacks_len,
        U64 knightBB, U64 friendPieceBB) {
    uint16_t tmpMove;
    uint16_t knightIndex;
    std::vector<U64> separatedMoves;
    U64 allBB = _board.getAllBB();

    // All possible moves of a given knight.
    U64 allPosMoves;

    while (knightBB) {
        // Get knight from bit board.
        knightIndex = getSquareIndex(knightBB);

        // Set source of moves this knight can do.
        tmpMove = knightIndex;

        allPosMoves = knightPosMoves[knightIndex] & (~friendPieceBB);

        U64 movesBB = allPosMoves & (~allBB);
        allPosMoves &= allBB;

        // Iterate through quiet moves
        separatedMoves = getSeparatedBits(movesBB);
        for (auto move : separatedMoves) {
            // Set the destination of this move.
            tmpMove |= (getSquareIndex(move) << 6);

            moves[(*len)++] = tmpMove;

            // Reset the destination for the next move.
            tmpMove &= 0x3f;
        }
        
        // Iterate through all attacks
        separatedMoves = getSeparatedBits(allPosMoves);
        for (auto move : separatedMoves) {
            // Set the destination of this move.
            tmpMove |= (getSquareIndex(move) << 6);

            attacks[(*attacks_len)++] = tmpMove;

            // Reset the destination for the next move.
            tmpMove &= 0x3f;
        }

        // Remove knight from bitboard.
        U64 one = 1;
        knightBB ^= (one << knightIndex);
    }
}


void Generator::whiteCastle(uint16_t* moves, uint16_t* len) {
    if ((_board.getFlags() & WHITEKINGSIDECASTLE) &&
        (_board.getAllBB() & 0x60) == 0 &&
        _board.getKingBB(whiteSide) == 0x10 &&
        (_board.getRookBB(whiteSide) & 0x80)) {
        // 0b1100 000110 000100
        moves[(*len)++] = 0xc184;
    }
    if ((_board.getFlags() & WHITEQUEENSIDECASTLE) &&
        (_board.getAllBB() & 0xe) == 0 &&
        _board.getKingBB(whiteSide) == 0x10 &&
        (_board.getRookBB(whiteSide) & 0x1)) {
        // 0b1100 000010 000100
        moves[(*len)++] = 0xc084;
    }
}

void Generator::blackCastle(uint16_t* moves, uint16_t* len) {
    if ((_board.getFlags() & BLACKKINGSIDECASTLE) &&
        (_board.getAllBB() & 0x6000000000000000) == 0 &&
        _board.getKingBB(blackSide) == 0x1000000000000000 &&
        (_board.getRookBB(blackSide) & 0x8000000000000000)) {
        // 0b1100 111110 111100
        moves[(*len)++] = 0xcfbc;
    }
    if ((_board.getFlags() & BLACKQUEENSIDECASTLE) &&
        (_board.getAllBB() & 0xe00000000000000) == 0 &&
        _board.getKingBB(blackSide) == 0x1000000000000000 &&
        (_board.getRookBB(blackSide) & 0x100000000000000)) {
        // 0b1100 111010 111100
        moves[(*len)++] = 0xcebc;
    }
}
