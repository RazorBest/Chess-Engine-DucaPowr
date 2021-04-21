/* Copyright 2021 DucaPowr Team */
#include "testGenerator.h"

U8 getLineAttacks(U8 rook, U8 occ) {
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

void testGenerator() {
    Board board;
    Generator generator(board);

    for (U8 rook_index = 0; rook_index < 8; rook_index++) {
        U8 rook = 1 << rook_index;
        for (U8 occ_index = 0; occ_index < 64; occ_index++) {
            U8 occ = occ_index << 1;
            if (generator.firstRankAttacks[occ_index][rook_index] != 
                    getLineAttacks(rook, occ)) {
                std::cerr << "Test failed\n" << "rook_index=" <<
                    (int)rook_index <<
                    "\nocc_index=" << (int)occ_index <<
                    "\ngenerator.firstRankAttack[rook_index][occ_index]=" <<
                    (int)generator.firstRankAttacks[occ_index][rook_index] <<
                    "\ngetLineAttacks(rook, occ)=" <<
                    (int)getLineAttacks(rook, occ) << '\n';
                assert(0);
            }
        }
    }
}
