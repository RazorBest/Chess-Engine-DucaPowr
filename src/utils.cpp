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
