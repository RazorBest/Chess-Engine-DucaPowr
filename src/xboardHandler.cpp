/* Copyright 2021 DucaPowr Team */
#include "./xboardHandler.h"

xBoardHandler::xBoardHandler(Engine& engine) : _engine(engine) { }

void xBoardHandler::init() {
    std::cout.setf(std::ios::unitbuf);
    std::string buffer;

    // RECV xboard
    std::getline(std::cin, buffer);
    if (buffer.compare("xboard") != 0) {
        _logger.error("xboard command expected - got " + buffer);
        exit(1);
    }

    // RECV protover N
    std::getline(std::cin, buffer);
    if (buffer.find("protover") == std::string::npos) {
        _logger.error("protover N command expected - got " + buffer);
        exit(1);
    }

    // SEND feature
    std::cout << "feature " + std::string(FEATURE_ARGS) << std::endl;
}

void xBoardHandler::run() {
    std::string buffer;
    std::getline(std::cin, buffer);

    _logger.info("xboard -> " + buffer);

    std::istringstream iss(buffer);
    std::string firstToken;

    std::getline(iss, firstToken, ' ');

    if (firstToken == "new") {
        _engine.newGame();

        // default engineRunning = true
        engineRunning = true;

    } else if (firstToken == "usermove") {
        std::string move;
        std::getline(iss, move, ' ');

        // opponent moved
        _engine.userMove(move);

        if (engineRunning) {
            // engine moves
            std::string move = "move " + _engine.move();
            std::cout <<  move << std::endl;
            _logger.info("xboard <- " + move);
        }

    } else if (firstToken == "go") {
        // engine moves
        std::string move = "move " + _engine.move();
        std::cout <<  move << std::endl;
        _logger.info("xboard <- " + move);

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
