/* Copyright 2021 DucaPowr Team */
#include "./xboardHandler.h"

xBoardHandler::xBoardHandler(Engine& engine) : _engine(engine) {
    std::string line;
    std::ifstream f(QUOETS_FILE);
    while (std::getline(f, line))
    {
        quotes.push_back(line);
    }
    f.close();
}

void xBoardHandler::init(void) {
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

void xBoardHandler::run(void) {
    std::string buffer;
    std::getline(std::cin, buffer);

    _logger.info("xboard -> " + buffer);

    std::istringstream iss(buffer);
    std::string firstToken;

    std::getline(iss, firstToken, ' ');

    if (firstToken == "new") {
        _engine.newGame();

        // default observing = true
        observing = true;

    } else if (firstToken == "usermove") {
        std::string move;
        std::getline(iss, move, ' ');

        // opponent moved
        _engine.userMove(move);

        if (observing) {
            // engine moves
            engineMove();
        }

    } else if (firstToken == "go") {
        // engine moves
        engineMove();

        // default observing = true
        observing = true;

    } else if (firstToken == "force") {
        // engine paused, just listen to input
        observing = false;

    } else if (firstToken == "quit") {
        // xboard stopped
        _engine.close();
    }
}

void xBoardHandler::engineMove(void) {
    std::string move = _engine.move();
    if (move == "resign") {
        move = getResignationString();
    } else {
        move = "move " + move;
    }
    std::cout <<  move << std::endl;
    _logger.info("xboard <- " + move);
}

std::string xBoardHandler::getResignationString(void) {
    unsigned int seed = static_cast <int64_t> (time(NULL));
    int randIndex = rand_r(&seed) % quotes.size();
    std::string result = (_engine.sideToMove()) ? "1-0" : "0-1";
    return result + " {" + "Duca resigns. " + quotes[randIndex] + "}";
}