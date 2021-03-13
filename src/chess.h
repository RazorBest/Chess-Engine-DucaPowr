#pragma once

#include <string>

class Chess {
    public:
        /**
         * Resets the game and makes engine play black.
         */
        void newGame();

        /**
         * Opponent moved
         * 
         * @param move san=0 encoding of the move; eg: e2e4
         */
        void userMove(std::string move);

        /**
         * Engine thinks and moves the color that has to move this turn
         * 
         * @return san=0 encoding of the move; eg: e2e4
         */
        std::string move();
};