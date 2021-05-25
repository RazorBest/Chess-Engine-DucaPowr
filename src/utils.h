/* Copyright 2021 DucaPowr Team */
#pragma once

#include <stdint.h>
#include <vector>

enum Side {
    whiteSide,
    blackSide
};

typedef uint64_t U64;
typedef uint8_t  U8;

#define DIE(condition, message) \
	do { \
		if ((condition)) { \
			fprintf(stderr, "[%d]: %s\n", __LINE__, (message)); \
			perror(""); \
			exit(1); \
		} \
	} while (0)

std::vector<U64> getSeparatedBits(U64 bb);

/**
 * Most Significant Bit bitscan
 * 
 * @param bb - a bitboard where at least 1 bit is set
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

/**
 * @brief This functions moves all the pieces on a bitboard south one time.
 * 
 * @param b the bitboard
 * @return U64 the shifted bitboard.
 */
U64 southOne(U64 b);

/**
 * @brief This functions moves all the pieces on a bitboard north one time.
 * 
 * @param b the bitboard
 * @return U64 the shifted bitboard.
 */
U64 northOne(U64 b);

/**
 * @brief This functions moves all the pieces on a bitboard east one time.
 * 
 * @param b the bitboard
 * @return U64 the shifted bitboard.
 */
U64 eastOne(U64 b);

/**
 * @brief This functions moves all the pieces on a bitboard west one time.
 * 
 * @param b the bitboard
 * @return U64 the shifted bitboard.
 */
U64 westOne(U64 b);

/**
 * @brief This functions moves all the pieces on a bitboard northeast one time.
 * 
 * @param b the bitboard
 * @return U64 the shifted bitboard.
 */
U64 northEastOne(U64 b);

/**
 * @brief This functions moves all the pieces on a bitboard southeast one time.
 * 
 * @param b the bitboard
 * @return U64 the shifted bitboard.
 */
U64 southEastOne(U64 b);

/**
 * @brief This functions moves all the pieces on a bitboard northwest one time.
 * 
 * @param b the bitboard
 * @return U64 the shifted bitboard.
 */
U64 northWestOne(U64 b);

/**
 * @brief This functions moves all the pieces on a bitboard southwest one time.
 * 
 * @param b the bitboard
 * @return U64 the shifted bitboard.
 */
U64 southWestOne(U64 b);

/**
 * @brief This function takes a bit board with only the king's position marked
 * and returns a bitboard with only the king's neighbors marked.
 *
 * @param kingBB a bit board with only the king's position marked
 * @return U64 a bitboard with only the king's neighbors marked.
 */
U64 aKingsNeighbors(U64 kingBB);

Side otherSide(Side side);