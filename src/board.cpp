#include <bits/stdint-uintn.h>

#include "board.h"
#include "constants.h"

enum enumPiece {
    nWhitePawn,
    nBlackPawn,
    nWhiteBishop,
    nBlackBishop,
    nWhiteKnight,
    nBlackKnight,
    nWhiteRook,
    nBlackRook,
    nWhiteQueen,
    nBlackQueen,
    nWhiteKing,
    nBlackKing
};

void Board::init() {
    pieceBB[nWhitePawn] = WHITEPAWNSTART;                                   
    pieceBB[nBlackPawn] = BLACKPAWNSTART;                                   
    pieceBB[nWhiteBishop] = WHITEBISHOPSTART;                               
    pieceBB[nBlackBishop] = BLACKBISHOPSTART;                               
    pieceBB[nWhiteKnight] = WHITEKNIGHTSTART;                               
    pieceBB[nBlackKnight] = BLACKKNIGHTSTART;                               
    pieceBB[nWhiteRook] = WHITEROOKSTART;                                   
    pieceBB[nBlackRook] = BLACKROOKSTART;                                   
    pieceBB[nWhiteQueen] = WHITEQUEENSTART;                                 
    pieceBB[nBlackQueen] = BLACKQUEENSTART;                                 
    pieceBB[nWhiteKing] = WHITEKINGSTART;                                   
    pieceBB[nBlackKing] = BLACKKINGSTART;
}

#pragma region Bitboard getters
U64 Board::getPieceBB(Side side) {
    U64 *BB = this->pieceBB;

    // Perform bitwise OR between all white pieces
    return  BB[nWhitePawn + side] | 
            BB[nWhiteBishop + side] | 
            BB[nWhiteKnight + side] | 
            BB[nWhiteRook + side] | 
            BB[nWhiteQueen + side] | 
            BB[nWhiteKing + side];
}

U64 Board::getPawnBB(Side side) {
    return pieceBB[nWhitePawn + side];
}

U64 Board::getBishopBB(Side side) {
    return pieceBB[nWhiteBishop + side];
}


U64 Board::getKnightBB(Side side) {
    return pieceBB[nWhiteKnight + side];
}

U64 Board::getRookBB(Side side) {
    return pieceBB[nWhiteRook + side];
}

U64 Board::getQueenBB(Side side) {
    return pieceBB[nWhiteQueen + side];
}

U64 Board::getKingBB(Side side) {
    return pieceBB[nWhiteKing + side];
}

U64 Board::getAllBB() {
    return getPieceBB(whiteSide) | getPieceBB(blackSide);
}
#pragma endregion

#pragma region Helpers
int Board::getPieceIndexFromSquare(uint16_t sq) {
    // Convert from index(0-63) to bitboard 
    U64 sqBB = 1 << sq;

    // Iterate through every bitboard
    for (size_t i = 0; i < sizeof(pieceBB) / sizeof(*pieceBB); i++) {
        if (pieceBB[i] & sqBB) {
            return i;
        }  
    }

    return -1;
}


#pragma endregion

#pragma region SAN Move Converters
uint16_t Board::convertSanToMove(std::string move) {
    // TODO - check special flags
    uint16_t src = ((move[1] - '1') << 3) + (move[0] - 'a');
    uint16_t dest = ((move[3] - '1') << 3) + (move[2] - 'a');
    uint8_t prom = 0;
    uint8_t flags = 0;
    if (move.size() == 5) {
        switch (move[4]) {
            case 'r':
                prom = 0;
                break;
            case 'n':
                prom = 1;
                break;
            case 'b':
                prom = 2;
                break;
            case 'q':
                prom = 3;
                break;
            default:
                break;
        }
    }
    return (flags << 14) + (prom << 12) + (src << 6) + dest;
}

std::string Board::convertMoveToSan(uint16_t move) {
    // TODO - barbu
}

#pragma endregion

std::vector<U64> Board::getSeparatedBits(U64 bb) {
    std::vector<U64> moves;

    while (bb) {
        moves.push_back(bb & -bb);
        bb &= bb - 1;
    }

    return moves;
}

// This is inefficient
uint16_t getSquareIndex(U64 bb) {
    int index = 0;

    while (bb >>= 1) {
        index++;
    }

    return index;
}

std::string Board::toString() {
    static char const pieceSymbol[12] = {'p', 'P', 'b', 'B', 'n', 'N',
        'r', 'R', 'q', 'Q', 'k', 'K'};
    static char const emptySymbol = '.';
    // char array map
    char board[64];

    memeset(board, emptySymbol, 64);

    // Store the pieces in the char array map
    for (size_t i = 0; i < sizeof(pieceBB) / sizeof(*pieceBB); i++) {
        auto pieces = getSeparatedBits(pieceBB[i]);
        for (U64 pieceBB : pieces) {
            int sqIndex = getSquareIndex(pieceBB);

            board[sqIndex] = pieceSymbol[i];
        } 
    }

    // Convert to std::string and add a newline after every 8 characters
    std::string output;
    for (int i = 0; i < 8; i++) {
        output += std::string(board + i*8, 8) + '\n'; 
    }

    return output;
}

/**
 * Change bitboards according to given move.
 * Returns whether the move is legal or not
 * TODO test function, add legality check
*/
bool Board::applyMove(uint16_t move) {
    uint16_t sourceSquare = move & 0xf;
    uint16_t destSquare = (move & 0xf0) >> 4;
    uint64_t sourcePosBoard = 1 << sourceSquare;
    uint64_t destPosBoard = 1 << destSquare;

    pieceBB[getPieceIndexFromSquare(sourceSquare)] ^= sourcePosBoard;
    pieceBB[getPieceIndexFromSquare(destSquare)] |= destPosBoard;

    return true; // TODO change later
}
