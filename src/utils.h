#pragma once

#include <stdint.h>
#include <vector>

typedef uint64_t U64;

std::vector<U64> getSeparatedBits(U64 bb);

uint16_t getSquareIndex(U64 bb);