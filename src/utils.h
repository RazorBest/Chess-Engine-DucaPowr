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

U8 reverse(U8);
