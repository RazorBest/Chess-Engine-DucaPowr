#include <iostream>
#include <string>
#include <map>

const char SIG_INT[] = "0";
const char SAN[] = "0";
const char NAME[] = "Duca";

std::map<const char*, const char*> options = {
    {"sigint", SIG_INT},
    {"san", SAN},
    {"name", NAME}
};

class Logger {
    private:
        int log_level;
        int default_log_level = 0;
    public:
        Logger(): log_level(0) {}
        
        Logger(int log_level): log_level(log_level) {}

        void log(std::string msg) {
            log(msg, default_log_level);
        }
        
        void log(std::string msg, int level) {
            if (level >= log_level) {
                std::cout << msg;
                std::cout.flush();
            } 
        }
};

Logger logger;

class Chess {
    public:
        void newGame() {
            logger.log("Starting new game...\n");
        }

        void move(std::string s) {
            logger.log("Move: " + s + '\n');
        }

        std::string getMove() {
            return "d7d5";
        }

        void update() {
        }
};

static int write_feature() {
    std::string input;

    std::cout << "feature";
    for (auto option : options) {
        std::cout << ' ' << option.first << '=' << option.second;    
    }
    std::cout << '\n';

    for (int i = 0; i < options.size(); i++) {
        std::getline(std::cin, input);
        
        if (input.find("rejected") == 0) {
            logger.log(input);
            return 1;
        }
    }

    return 0;
}

int main() {
    std::string input;
    Chess chess;

    //std::cout.setf(std::ios::unitbuf);
    
    std::getline(std::cin, input);
    if (input != "xboard") {
        logger.log("xboard didn't start\n");
        return 1;
    }

    std::getline(std::cin, input);
    // If string doesn't start with "protover"
    if (input.find("protover") != 0) {
        logger.log("protover command wasn't sent\n");
        return 1;
    }
    //TODO Check protover argument

    write_feature();

    logger.log("Start loop\n");

    while (1) {
        std::getline(std::cin, input);
      
        if (input == "new") {
            chess.newGame();
        } else if (input == "hard") {
        } else if (input[0] >= 'a' && input[0] <= 'h') {
            chess.move(input);
            std::cout << "move " + chess.getMove() + '\n';
        }
    }

    return 0;
}
