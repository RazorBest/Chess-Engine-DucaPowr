/* Copyright 2021 DucaPowr Team */
#include "./engine.h"
#include "./logger.h"

#include <istream>
#include <time.h>
#include <climits>

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

    if (DEBUG) {
        _logger.raw(_board.toString() + '\n');
    }
}

/**
 * Engine thinks and moves the color that has to move this turn
 *
 * @return SAN=0 encoding of the move
 */
std::string Engine::move(void) {

    uint16_t move = 0xffff;

    int depth = 6;

    int score = alphaBetaMax(INT_MIN, INT_MAX, depth, &move);

    _board.applyMove(move);

    if (DEBUG) {
        _logger.raw(_board.toString() + '\n');
        _logger.raw("Score for move: " + std::to_string(score));
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


#include <cassert>
// ALPHA-BETA
int Engine::alphaBetaMax(int alpha, int beta, int depthleft, uint16_t *move) {
    // if i'm last node return my eval
    if ( depthleft == 0 ) {
        return (_board.eval());
    }

    uint16_t moves[MAX_MOVES_AT_STEP];
    uint16_t movesLen = 0;
    uint16_t garbage;
    // generate moves
    _generator.generateMoves(moves, &movesLen);

    // todo sort moves (here or in generateMoves)

    int score = INT_MIN;
    uint16_t currMove;
    for (int i = 0; i < movesLen; ++i) {
        currMove = moves[i];
        
        U64 boardCopy[14];
        memcpy(boardCopy, _board.pieceBB, 14 * sizeof(U64));


        // apply move
        _board.applyMove(currMove);

        // search deeper
        score = alphaBetaMin(alpha, beta, depthleft - 1, &garbage);

        // undo move
        _board.undoMove();


        U64 boardCopy2[14];
        memcpy(boardCopy2, _board.pieceBB, 14 * sizeof(U64));

        if (memcmp(boardCopy, boardCopy2, 12 * sizeof(U64))) {
            _logger.raw("After undo:");
            _logger.raw(_board.toString());

            for (int j = 0; j < 12; j++) {
                _logger.logBB(boardCopy[j]);
                _logger.logBB(boardCopy2[j]);
            }

            Logger::debugFile.close();

            assert(0);
        }


        if( score >= beta ) {
            return beta;   // fail hard beta-cutoff
        }
        if( score > alpha ) {
            alpha = score; // alpha acts like max in MiniMax
            *move = currMove;
        }
    }
    return alpha;
}

int Engine::alphaBetaMin(int alpha, int beta, int depthleft, uint16_t *move) {
    // if i'm last node return my eval
    if ( depthleft == 0 ) {
        return -(_board.eval());
    }

    uint16_t moves[MAX_MOVES_AT_STEP];
    uint16_t movesLen = 0;
    // generate moves
    _generator.generateMoves(moves, &movesLen);

    // todo sort moves (here or in generateMoves)

    int score = INT_MAX;
    uint16_t currMove;
    for (int i = 0; i < movesLen; ++i) {
        currMove = moves[i];

        U64 boardCopy[14];
        memcpy(boardCopy, _board.pieceBB, 14 * sizeof(U64));

        // apply move
        _board.applyMove(currMove);

        U64 boardCopy3[14];
        memcpy(boardCopy3, _board.pieceBB, 14 * sizeof(U64));


        // search deeper
        score = alphaBetaMax( alpha, beta, depthleft - 1, move);
    
        U64 boardCopy4[14];
        memcpy(boardCopy4, _board.pieceBB, 14 * sizeof(U64));

        // undo move
        _board.undoMove();

        U64 boardCopy2[14];
        memcpy(boardCopy2, _board.pieceBB, 14 * sizeof(U64));

        if (memcmp(boardCopy, boardCopy2, 12 * sizeof(U64))) {

            _logger.raw("Error in alphaBetaMin");
            _logger.raw("After undo:");
            _logger.raw(_board.toString());


            for (int j = 0; j < 12; j++) {
                _logger.raw("Piece " + std::to_string(j) + "\n\n");
                _logger.logBB(boardCopy[j]);
                _logger.logBB(boardCopy3[j]);
                _logger.logBB(boardCopy4[j]);
                _logger.logBB(boardCopy2[j]);

                char moveStr[20] = {0};
                sprintf(moveStr, "0x%hx", currMove);

                _logger.raw("Move: " + std::string(moveStr) + "\n");
            }

            Logger::debugFile.close();

            assert(0);
        }

        if( score <= alpha ) {
            return alpha; // fail hard alpha-cutoff
        }
        if( score < beta ) {
            beta = score; // beta acts like min in MiniMax
        }
    }
    return beta;
}
