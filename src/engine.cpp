#include "engine.h"

#include <time.h>

/**
 * Resets the game and makes engine play black.
 */
void Engine::newGame() {
    board.init();
}

/**
 * Opponent moved
 * 
 * @param move san=0 encoding of the move
 */
void Engine::userMove(std::string move) {
    board.applyMove(board.convertSanToMove(move));
}

/**
 * Engine thinks and moves the color that has to move this turn
 * 
 * @return san=0 encoding of the move
 */
std::string Engine::move() {
    // this only for etapa 1 - generate random pseudo-legal pawn move
    uint16_t moves[MAX_MOVES_AT_STEP];
    memset(moves, 0, sizeof(moves));

    uint16_t moves_len = 0;

    generator.generateMoves(board, moves, &moves_len);

    srand(time(NULL));

    int rand_index = rand() % moves_len;
    uint16_t move = moves[rand_index];

    board.applyMove(move);

    return board.convertMoveToSan(move);
}