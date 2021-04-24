/* Copyright 2021 DucaPowr Team */
#include "./utils.h"

std::vector<U64> getSeparatedBits(U64 bb) {
    std::vector<U64> moves;

    while (bb) {
        moves.push_back(bb & -bb);
        bb &= bb - 1;
    }

    return moves;
}

// TODO(razorbest) - This is inefficient
uint16_t getSquareIndex(U64 bb) {
    int index = 0;

    while (bb >>= 1) {
        index++;
    }

    return index;
}

U64 flipVertical(U64 x) {
    return __builtin_bswap64(x);
}

/**
 * Copied from chessprogramming.com
 *
 * Flip a bitboard about the diagonal a1-h8.
 * Square h1 is mapped to a8 and vice versa.
 * @param x any bitboard
 * @return bitboard x flipped about diagonal a1-h8
 */
U64 flipDiagA1H8(U64 x) {
    U64 t;
    const U64 k1 = 0x5500550055005500;
    const U64 k2 = 0x3333000033330000;
    const U64 k4 = 0x0f0f0f0f00000000;
    t  = k4 & (x ^ (x << 28));
    x ^=       t ^ (t >> 28);
    t  = k2 & (x ^ (x << 14));
    x ^=       t ^ (t >> 14);
    t  = k1 & (x ^ (x <<  7));
    x ^=       t ^ (t >>  7);
    return x;
}

/**
 * Copied from chessprogramming.com
 *
 * Flip a bitboard about the antidiagonal a8-h1.
 * Square a1 is mapped to h8 and vice versa.
 * @param x any bitboard
 * @return bitboard x flipped about antidiagonal a8-h1
 */
U64 flipDiagA8H1(U64 x) {
    U64 t;
    const U64 k1 = 0xaa00aa00aa00aa00;
    const U64 k2 = 0xcccc0000cccc0000;
    const U64 k4 = 0xf0f0f0f00f0f0f0f;
    t  =       x ^ (x << 36);
    x ^= k4 & (t ^ (x >> 36));
    t  = k2 & (x ^ (x << 18));
    x ^=       t ^ (t >> 18);
    t  = k1 & (x ^ (x <<  9));
    x ^=       t ^ (t >>  9);
    return x;
}

U64 rotate90Clockwise(U64 x) {
    return flipVertical(flipDiagA1H8(x));
}

U64 rotate90AntiClockwise(U64 x) {
    return flipDiagA1H8(flipVertical(x));
}

U8 reverse(U8 byte) {
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}

U64 rankBB(uint16_t rankIndex) {
    U64 rank = 1;

    rank <<= (rankIndex * 8);
    rank |= rank << 1;
    rank |= rank << 2;
    rank |= rank << 4;

    return rank;
}

U64 fileBB(uint16_t fileIndex) {
    U64 file = 1;

    file <<= fileIndex;
    file |= file << 8;
    file |= file << 16;
    file |= file << 32;

    return fileIndex;
}
