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

U64 Board::getEnPassantablePawnsBB(Side side) {
    /**
     * Note: Side::whiteSide = 0 and Side::blackSide = 1;
     * << (sideToMove << 3) shifts the flags to match the appropriate side;
     * Flag bits [0..7] or [8..15] are a side's respective pawns that just
     * jumped.
    */
    return (flags & (0xffLL << (sideToMove << 3))) << 24;
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

void Board::switchSide(void) {
    if (sideToMove == Side::whiteSide) {
        sideToMove = Side::blackSide;
    } else {
        sideToMove = Side::whiteSide;
    }
}

U64 Board::getFlags(void) {
    return flags;
}

#pragma endregion

#pragma region SAN Move Converters
uint16_t Board::convertSanToMove(std::string move) {
    logger.raw("Let's see " + move);
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

std::string Board::toString(void) {
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

// TODO: Add inline if it works.
void Board::resetEnPassant(void) {
    // Note: Side::whiteSide = 0 and Side::blackSide = 1.
    flags &= (~(0xffLL << (sideToMove << 3)));
}

// TODO: Add inline if it works.
void Board::setEnPassant(uint16_t move) {
    // Get whether a pawn is en passant-able. Also acts as a pseudo if.
    // Check move format in moveGen.h for more info.
    const long long enPassant = ((move >> 14) == 2);

    /**
     * Set side specific, pawn specific flag.
     * Note: Side::whiteSide = 0 and Side::blackSide = 1;
     * (move & 0x3f) % 8) gets the file of the pawn (0 is a, 7 is h);
     * << (sideToMove << 3) shifts the flag to match the appropriate side.
     * TODO: test if (move % 8) is enough, instead of ((move & 0x3f) % 8).
    */
    flags |= ((enPassant << ((move & 0x3f) % 8)) << (sideToMove << 3));
}

// TODO: Add inline if it works.
void Board::enPassantAttackPrep(uint16_t move) {
    uint16_t destSquare = (move >> 6) & 0x3f;
    U64 destPosBoard = 1;
    destPosBoard <<= destSquare;

    /**
     * Get the current position bitBrd of the attacked pawn, colour dependent: 
     * White pawns will be one rank higher, black pawns will be one rank lower.
     * 
     * Also acts as a pseudo if, checking the en passant-able flag.
     * Note: flags >> ((1 - sideToMove) << 3) gets the en passant flags for the
     * side opposite of the one to attack.
     * Side::whiteSide = 0 and Side::blackSide = 1;
     * >> (destSquare % 8) gets the file of the pawn being attacked;
     * & 1 ignores the other flags;
     * << destSquare transforms the bit into a bitboard.
    */
    U64 srcPosBoard = ((( (flags >> ((1 - sideToMove) << 3))
                        >> (destSquare % 8)) & 1) << destSquare);
    // Shift in case of a white pawn.
    srcPosBoard <<= ((1 - sideToMove) << 3);
    // Shift in case of a black pawn.
    srcPosBoard >>= ((sideToMove) << 3);

    // Also acts as a pseudo if thanks to the trash piece optimization.
    enum enumPiece sourceSquareIndex = getPieceIndexFromSquare(
        getSquareIndex(srcPosBoard));

    // Remove pawn from its current position.
    pieceBB[sourceSquareIndex] ^= srcPosBoard;

    // Add pawn to its en passant capture position.
    pieceBB[sourceSquareIndex] |= destPosBoard;
}

// TODO: test function, add legality check.
// Also do castling and promotion.
bool Board::applyMove(uint16_t move) {
    // Note: this function works with an internal pseudo if of sorts which may
    // or may not be faster since no jumps are made.
    enPassantAttackPrep(move);
    resetEnPassant();

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

    // Note: this function works with an internal pseudo if of sorts which may
    // or may not be faster since no jumps are made.
    setEnPassant(move);

    switchSide();

    logger.raw(toString() + '\n');

    // TODO(all) change later with move legality.
    return true;
}

// TODO do castling, en passant and promotion.
bool Board::undoMove(void) {
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

    DIE(takeHistory.empty(), "Error in undoMove(): takeHistory and moveHistory\
     stacks have different sizes!");
    enum enumPiece destSquareIndex = takeHistory.top();
    takeHistory.pop();

    // Remove source piece from the destination position on the source board.
    pieceBB[sourceSquareIndex] ^= destPosBoard;

    // Add destination piece back to its board.
    pieceBB[destSquareIndex] |= destPosBoard;

    // Add source piece back to its initial place on its board.
    pieceBB[sourceSquareIndex] |= sourcePosBoard;

    return true;
}
