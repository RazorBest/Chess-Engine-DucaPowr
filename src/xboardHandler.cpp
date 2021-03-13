#include "xboardHandler.h"

xBoardHandler::xBoardHandler(Engine& engine) {
    engine = engine;
}

void xBoardHandler::init() {
    std::cout.setf(std::ios::unitbuf);
    std::string buffer;

    // RECV xboard
    std::getline(std::cin, buffer);
    if (buffer.compare("xboard") != 0) {
        logger.error("xboard command expected - got " + buffer);
        exit(1);
    }

    // RECV protover N
    std::getline(std::cin, buffer);
    if (buffer.find("protover") == std::string::npos) {
        logger.error("protover N command expected - got " + buffer);
        exit(1);
    }

    // SEND feature
    std::cout << "feature sigint=0 san=0 name=DucaPowr colors=0 usermove=1 done=1" << std::endl;
}

void xBoardHandler::run() {
    std::string buffer;
    std::getline(std::cin, buffer);

    logger.info("xboard -> " + buffer);

    std::istringstream iss(buffer);
    std::string firstToken;

    std::getline(iss, firstToken, ' ');

    if (firstToken == "new") {
        engine.newGame();

        // default engineRunning = true
        engineRunning = true;

    } else if (firstToken == "usermove") {
        std::string move;
        std::getline(iss, move, ' ');

        // opponent moved
        engine.userMove(move);

        if (engineRunning) {
            // engine moves
            std::string move = "move " + engine.move();
            std::cout <<  move << std::endl;
            logger.info("xboard <- " + move);
        }

    } else if (firstToken == "go") {
        // engine moves
        std::string move = "move " + engine.move();
        std::cout <<  move << std::endl;
        logger.info("xboard <- " + move);

        // default engineRunning = true
        engineRunning = true;

    } else if (firstToken == "force") {
        // engine paused, just listen to input
        engineRunning = false;

    } else if (firstToken == "quit") {
        // xboard stopped
        engineRunning = false;
    }
}