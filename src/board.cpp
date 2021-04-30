/* Copyright 2021 DucaPowr Team */
#include "./board.h"

#include <bits/stdint-uintn.h>

#include "./constants.h"

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

    sideToMove = whiteSide;
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

U64 Board::getEmptyBB() {
    return ~getAllBB();
}
#pragma endregion

#pragma region Helpers
enum enumPiece Board::getPieceIndexFromSquare(uint16_t sq) {
    // Convert from index(0-63) to bitboard
    U64 sqBB = 1;
    sqBB <<= sq;

    // Iterate through every bitboard
    for (size_t i = 0; i < 12; i++) {
        if (pieceBB[i] & sqBB) {
            return (enum enumPiece) i;
        }
    }

    return trashPiece;
}

void Board::switchSide() {
    if (sideToMove == Side::whiteSide) {
        sideToMove = Side::blackSide;
    } else {
        sideToMove = Side::whiteSide;
    }
}
#pragma endregion

#pragma region SAN Move Converters
uint16_t Board::convertSanToMove(std::string move) {
    uint16_t res = ((move[3] - '1') << 3) | (move[2] - 'a');
    res <<= 6;
    res |= ((move[1] - '1') << 3) | (move[0] - 'a');

    if (move.size() == 5) {
        res |= 0x3000;
        switch (move[4]) {
            case 'r':
                break;
            case 'n':
                res |= 0x1000;
                break;
            case 'b':
                res |= 0x2000;
                break;
            case 'q':
                res |= 0x3000;
                break;
            default:
                break;
        }
    }
    return res;
}

std::string Board::convertMoveToSan(uint16_t move) {
  std::string res;
  res.push_back((move & 7) + 'a');
  res.push_back(((move >> 3) & 7) + '1');
  res.push_back(((move >> 6) & 7) + 'a');
  res.push_back(((move >> 9) & 7) + '1');

  if (move & 0x3000) {
    char prom;
    switch ((move & 0x3000) >> 12) {
    case 0:
      prom = 'r';
      break;
    case 1:
      prom = 'n';
      break;
    case 2:
      prom = 'b';
      break;
    case 3:
      prom = 'q';
      break;
    default:
      break;
    }
    res.push_back(prom);
    }
    return res;
}

#pragma endregion

std::string Board::toString() {
    static char const pieceSymbol[12] = {'p', 'P', 'b', 'B', 'n', 'N',
        'r', 'R', 'q', 'Q', 'k', 'K'};
    static char const emptySymbol = '.';
    // char array map
    char board[64];

    memeset(board, emptySymbol, 64);

    // Store the pieces in the char array map
    for (size_t i = 0; i < 12; i++) {
        auto pieces = getSeparatedBits(pieceBB[i]);
        for (U64 pieceBB : pieces) {
            int sqIndex = getSquareIndex(pieceBB);

            board[sqIndex] = pieceSymbol[i];
        }
    }

    // Convert to std::string and add a newline after every 8 characters
    std::string output;
    for (int i = 7; i >= 0; i--) {
        output += std::string(board + i*8, 8) + '\n';
    }

    return output;
}

// TODO test function, add legality check.
bool Board::applyMove(uint16_t move) {
    uint16_t sourceSquare = move & 0x3f;
    uint16_t destSquare = (move >> 6) & 0x3f;

    // Pos = position.
    U64 sourcePosBoard = 1;
    sourcePosBoard <<= sourceSquare;
    U64 destPosBoard = 1;
    destPosBoard <<= destSquare;


    enum enumPiece sourceSquareIndex = getPieceIndexFromSquare(sourceSquare);
    enum enumPiece destSquareIndex = getPieceIndexFromSquare(destSquare);

    // remove source piece from its bb
    pieceBB[sourceSquareIndex] ^= sourcePosBoard;

    // remove dest piece from its bb
    pieceBB[destSquareIndex] ^= destPosBoard;

    // add source piece to dest pos in source bb
    pieceBB[sourceSquareIndex] |= destPosBoard;

    moveHistory.push(move);
    takeHistory.push(destSquareIndex);

    switchSide();

    logger.raw(toString() + '\n');

    // TODO(all) change later with move legality.
    return true;
}

bool Board::undoMove() {
    if (moveHistory.empty()) {
        return false;
    }

    uint16_t move = moveHistory.top();
    moveHistory.pop();

    uint16_t sourceSquare = move & 0x3f;
    uint16_t destSquare = (move >> 6) & 0x3f;

    // Pos = position.
    U64 sourcePosBoard = 1;
    sourcePosBoard <<= sourceSquare;
    U64 destPosBoard = 1;
    destPosBoard <<= destSquare;

    enum enumPiece sourceSquareIndex = getPieceIndexFromSquare(destSquare);

    DIE(takeHistory.empty(), "Error in undoMove(): takeHsitory and moveHistory\
     stacks have different sizes!");
    enum enumPiece destSquareIndex = takeHistory.top();
    takeHistory.pop();

    // Remove source piece from the destination position on the source board.
    pieceBB[sourceSquareIndex] ^= destPosBoard;

    // Add destination piece back to its board.
    pieceBB[destSquareIndex] |= destPosBoard;

    // Add source piece back to its inital place on its board.
    pieceBB[sourceSquareIndex] |= sourcePosBoard;

    return true;
}
