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

    // now its my turn to move
    switchSide();
}

/**
 * Engine thinks and moves the color that has to move this turn
 * 
 * @return san=0 encoding of the move
 */
std::string Engine::move() {
    // this only for etapa 1 - generate random pseudo-legal pawn move
    uint16_t moves[MAX_MOVES_AT_STEP];

    uint16_t moves_len = 0;

    generator.generateMoves(board, moves, &moves_len);

    // TODO - change this

    //int rand_index = rand() % moves_len;

    //return board.convertMoveToSan(moves[rand_index]);

    // now its opponent's turn to move
    switchSide();

    return "e7e5";
}

void Engine::switchSide() {
    if (side_to_move == Side::whiteSide) {
        side_to_move = Side::blackSide;
    } else {
        side_to_move = Side::whiteSide;
    }
}