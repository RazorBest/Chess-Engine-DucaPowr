/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdint.h>
#include <vector>

typedef uint64_t U64;
typedef uint8_t  U8;

std::vector<U64> getSeparatedBits(U64 bb);

/**
 * @param bb - a bitboard where only 1 bit is set
 * @return - the index of the bit in LERF mapping
 */
uint16_t getSquareIndex(U64 bb);


U64 flipVertical(U64 x);
U64 flipDiagA1H8(U64 x);
U64 flipDiagA8H1(U64 x);
U64 rotate90Clockwise(U64 x);
U64 rotate90AntiClockwise(U64 x);

U8 reverse(U8);

/**
 * @param rankIndex - the index of the rank (0-7)
 * @return - a bitboard with the bits set on the specified rank
 */
U64 rankBB(uint16_t rankIndex);

/**
 * @param fileIndex - the index of the file (0-7)
 * @return - a bitboard with the bits set on the specified file
 */
U64 fileBB(uint16_t fileIndex);
