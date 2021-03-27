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
    uint64_t promotions;
    uint64_t possible_2_moves;

    possible_moves = (board.getPawnBB(whiteSide) << 8) & (~RANK8) & emptyPiece;
    promotions = (board.getPawnBB(whiteSide) << 8) & RANK8 & emptyPiece;
    possible_2_moves = (possible_moves << 8) & RANK4 & emptyPiece;

    std::vector<U64> separated = getSeparatedBits(possible_moves);
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
    uint64_t promotions;
    uint64_t possible_2_moves;
    
    possible_moves = (board.getPawnBB(blackSide) >> 8) & (~RANK1) & emptyPiece;
    promotions = (board.getPawnBB(blackSide) >> 8) & RANK1 & emptyPiece;
    possible_2_moves = (possible_moves >> 8) & RANK5 & emptyPiece;

    std::vector<U64> separated = getSeparatedBits(possible_moves);
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

void Generator::blackPawnAttacks(Board board, uint16_t* moves, uint16_t* len) {
    U64 opponentBB = board.getPieceBB(whiteSide);
    U64 pawnBB = board.getPawnBB(blackSide);
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
