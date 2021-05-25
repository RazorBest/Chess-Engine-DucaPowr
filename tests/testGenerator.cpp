/* Copyright 2021 DucaPowr Team */
#include "testGenerator.h"

static U8 getLineAttacks(U8 rook, U8 occ) {
    U8 attacks = 0;
    U8 rook_move = rook;

    if (rook & occ) {
        return 0;
    }

    // Generate left side attacks
    rook_move <<= 1;
    while (rook_move) {
        attacks |= rook_move;
        if (rook_move & occ) {
            break;
        }
        rook_move <<= 1;
    }

    // Generate right side attacks
    rook_move = rook >> 1;
    while (rook_move) {
        attacks |= rook_move;
        if (rook_move & occ) {
            break;
        }
        rook_move >>= 1;
    }


    return attacks;
}

static void testFirstRankAttacks(Generator &generator) {
    for (U8 rook_index = 0; rook_index < 8; rook_index++) {
        U8 rook = 1 << rook_index;
        for (U8 occ_index = 0; occ_index < 64; occ_index++) {
            U8 occ = occ_index << 1;
            if (generator.firstRankAttacks[occ_index][rook_index] !=
                    getLineAttacks(rook, occ)) {
                std::cerr << "Test failed\n" << "rook_index=" <<
                    static_cast<int>(rook_index) <<
                    "\nocc_index=" << static_cast<int>(occ_index) <<
                    "\ngenerator.firstRankAttack[rook_index][occ_index]=" <<
                    static_cast<int>
                    (generator.firstRankAttacks[occ_index][rook_index]) <<
                    "\ngetLineAttacks(rook, occ)=" <<
                    static_cast<int>(getLineAttacks(rook, occ)) << '\n';
                assert(0);
            }
        }
    }
}

static U64 getFileAttacks(U64 rook, U64 occ) {
    U64 attacks = 0;
    U64 rook_move = rook;

    if (rook & occ) {
        return 0;
    }

    // Generate top side attacks
    rook_move <<= 8;
    while (rook_move) {
        attacks |= rook_move;
        if (rook_move & occ) {
            break;
        }
        rook_move <<= 8;
    }

    // Generate bottom side attacks
    rook_move = rook >> 8;
    while (rook_move) {
        attacks |= rook_move;
        if (rook_move & occ) {
            break;
        }
        rook_move >>= 8;
    }

    return attacks;
}

static void testFirstFileAttacks(Generator &generator) {
    for (U8 rook_index = 0; rook_index < 8; rook_index++) {
        U64 rook = 1;
        rook <<= (rook_index * 8);
        for (U8 occ_index = 0; occ_index < 64; occ_index++) {
            U64 occ = occ_index << 1;
            occ = occ << 56;
            occ = rotate90AntiClockwise(occ);
            if (generator.firstFileAttacks[occ_index][rook_index] !=
                    getFileAttacks(rook, occ)) {
                std::cerr << "Test failed\n" << "rook_index=" <<
                    static_cast<int>(rook_index) <<
                    "\nocc_index=" << static_cast<int>(occ_index) <<
                    "\ngenerator.firstFileAttack[rook_index][occ_index]=" <<
                    generator.firstFileAttacks[occ_index][rook_index] <<
                    "\ngetFileAttacks(rook, occ)=" <<
                    getFileAttacks(rook, occ) << '\n';
                assert(0);
            }
        }
    }
}

void testGenerator(void) {
    Board board;
    Generator generator(board);

    std::cout << "testFirstRankAttacks()\n";
    std::cout.flush();
    testFirstRankAttacks(generator);
    std::cout << "DONE\n";

    std::cout << "testFirstFileAttacks()\n";
    std::cout.flush();
    testFirstFileAttacks(generator);
    std::cout << "DONE\n";
}
