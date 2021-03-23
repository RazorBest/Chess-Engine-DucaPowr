#include "moveGen.h"

void Generator::generateMoves(Board board, uint16_t* moves, uint16_t* cnt) {
    pawnMoves(board, moves, cnt);
}

void Generator::pawnMoves(Board board, uint16_t* moves, uint16_t* cnt) {
    logger.info("please move a pawn... :(");
}
