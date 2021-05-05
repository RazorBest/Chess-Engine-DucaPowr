/* Copyright 2021 DucaPowr Team */
#include "./engine.h"

#include <time.h>

/**
 * Resets the game and makes engine play black.
 */
void Engine::newGame() {
    _board.init();
    running = true;
}

/**
 * Opponent moved
 *
 * @param move san=0 encoding of the move
 */
void Engine::userMove(std::string move) {
    _board.applyMove(_board.convertSanToMove(move));
}

/**
 * Engine thinks and moves the color that has to move this turn
 *
 * @return SAN=0 encoding of the move
 */
std::string Engine::move() {
    // this only for stage 1 - generate random pseudo-legal pawn move
    // init
    uint16_t moves[MAX_MOVES_AT_STEP];
    memset(moves, 0, sizeof(moves));
    uint16_t movesLen = 0;

    // generate moves
    _generator.generateMoves(moves, &movesLen);

    // if i cant move, resign
    if (!movesLen) {
        // resign
        return "resign";
    }

    // chose random move
    unsigned int seed = static_cast <int64_t> (time(NULL));
    int randIndex = rand_r(&seed) % movesLen;
    uint16_t move = moves[randIndex];

    // apply move
    _board.applyMove(move);

    // return san representation
    return _board.convertMoveToSan(move);
}

void Engine::close() {
    running = false;
}

bool Engine::isRunning() {
    return running;
}

Side Engine::sideToMove() {
    return _board.sideToMove;
}