#include "engine.h"

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
    // todo
}

/**
 * Engine thinks and moves the color that has to move this turn
 * 
 * @return san=0 encoding of the move
 */
std::string Engine::move() {
    // this only for etapa 1 - generate random pseudo-legal pawn move
    uint16_t moves[MAX_MOVES_AT_STEP];

    generator.generateMoves(board, moves, 0);

    return "e7e5";
}