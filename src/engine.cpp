/* Copyright 2021 DucaPowr Team */
#include "./engine.h"
#include "./logger.h"

#include <time.h>

/**
 * Resets the game and makes engine play black.
 */
void Engine::newGame(void) {
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
std::string Engine::move(void) {
    // this only for stage 1 - generate random pseudo-legal pawn move
    // init
    uint16_t moves[MAX_MOVES_AT_STEP];
    uint16_t movesOtherSide[MAX_MOVES_AT_STEP];
    memset(moves, 0, sizeof(moves));
    uint16_t movesLen = 0, movesOtherSideLen = 0;

    // CASTLING ------------------------
    _generator.generateCastlingMoves(moves, &movesLen);
    uint16_t move = 0xffff;

    for (int i = 0; i < movesLen; i++) {
        move = moves[i];

        // apply move
        _board.applyMove(move);

        // generate all moves from enemy
        movesOtherSideLen = 0;
        _generator.generateMovesWithoutKing(movesOtherSide, &movesOtherSideLen);

        // check if move is illegal
        if (_checker.isLegal(move, movesOtherSide, movesOtherSideLen))
            break;

        _board.undoMove();
        move = 0xffff;
    }
    if (move != 0xffff) {
        return _board.convertMoveToSan(move);
    }



    // REST ----------------------------
    // generate moves
    movesLen = 0;
    _generator.generateMoves(moves, &movesLen);

    // if i cant move, resign
    if (!movesLen) {
        // resign
        return "resign";
    }

    move = 0xffff;

    // shuffle moves
    // To obtain a time-based seed
    unsigned int seed = static_cast <int64_t> (time(NULL));

    // Shuffling our array
    std::shuffle(moves, moves + movesLen, std::default_random_engine(seed));

    for (int i = 0; i < movesLen; i++) {
        move = moves[i];

        // apply move
        _board.applyMove(move);

        // generate all moves from enemy
        movesOtherSideLen = 0;
        _generator.generateMovesWithoutKing(movesOtherSide, &movesOtherSideLen);

        // check if move is illegal
        if (_checker.isLegal(move, movesOtherSide, movesOtherSideLen))
            break;

        _board.undoMove();
        move = 0xffff;
    }

    if (move == 0xffff) {
        // mate
        return "resign";
    }

    // return san representation
    return _board.convertMoveToSan(move);
}

void Engine::close(void) {
    running = false;
}

bool Engine::isRunning(void) {
    return running;
}

Side Engine::sideToMove(void) {
    return _board.sideToMove;
}