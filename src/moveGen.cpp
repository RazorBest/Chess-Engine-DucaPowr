#include "moveGen.h"

Generator::Generator(Board& board) {
    board = board;
}

void Generator::generateMoves(Board board, uint16_t* moves, uint16_t* len) {
    if (board.side_to_move == whiteSide) {
        whitePawnMoves(board, moves, len);
        whitePawnAttacks(board, moves, len);
    } else {
        blackPawnMoves(board, moves, len);
        blackPawnAttacks(board, moves, len);
    }
}

void Generator::whitePawnMoves(Board board, uint16_t* moves, uint16_t *len) {
    uint64_t emptyPiece = board.getEmptyBB();
    uint64_t possible_moves;
    uint64_t possible_2_moves;

    possible_moves = (board.getPawnBB(whiteSide) << 8) & emptyPiece; 
    possible_2_moves = (possible_moves << 8) & RANK4 & emptyPiece;

    std::vector<U64> separated = getSeparatedBits(possible_moves);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst >> 8); 

        moves[(*len)++] = tmp;
    }

    separated = getSeparatedBits(possible_2_moves);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst >> 16); 

        moves[(*len)++] = tmp;
    }
}

void Generator::blackPawnMoves(Board board, uint16_t* moves, uint16_t* len) {
    uint64_t emptyPiece = board.getEmptyBB();
    uint64_t possible_moves;
    uint64_t possible_2_moves;
    
    possible_moves = (board.getPawnBB(blackSide) >> 8) & emptyPiece; 
    possible_2_moves = (possible_moves >> 8) & RANK5 & emptyPiece;

    std::vector<U64> separated = getSeparatedBits(possible_moves);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst << 8); 

        moves[(*len)++] = tmp;
    }

    separated = getSeparatedBits(possible_2_moves);
    for (auto dst : separated) {
        uint16_t tmp = 0;
        tmp = getSquareIndex(dst);
        tmp = tmp << 6;
        tmp |= getSquareIndex(dst << 16); 

        moves[(*len)++] = tmp;
    }
}

void Generator::whitePawnAttacks(Board board, uint16_t* moves, uint16_t* len) {
    U64 opponentBB = board.getPieceBB(blackSide);
    U64 pawnBB = board.getPawnBB(whiteSide);
    U64 leftAttacks;
    U64 rightAttacks;

    leftAttacks = (pawnBB << 7) & (~HFILE) & opponentBB;
    rightAttacks = (pawnBB << 9) & (~AFILE) & opponentBB;

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
}

void Generator::blackPawnAttacks(Board board, uint16_t* moves, uint16_t* len) {
    U64 opponentBB = board.getPieceBB(whiteSide);
    U64 pawnBB = board.getPawnBB(blackSide);
    U64 leftAttacks;
    U64 rightAttacks;

    leftAttacks = (pawnBB >> 9) & (~HFILE) & opponentBB;
    rightAttacks = (pawnBB >> 7) & (~AFILE) & opponentBB;

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
}
