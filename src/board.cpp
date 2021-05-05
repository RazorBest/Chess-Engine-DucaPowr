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

/**
 * @brief This function converts a SAN=0 move to internal representation that
 * is explained in both README.md and moveGen.h.
 * 
 * @param move the source file and rank is written on the first positions of
 * the array, respectively the destination on the last. On the last position
 * a promotion is marked.
 * @return uint16_t an internal encoding.
 */
uint16_t Board::convertSanToMove(std::string move) {
    uint16_t dst = ((move[3] - '1') << 3) | (move[2] - 'a');
    uint16_t res = dst << 6;
    uint16_t src = ((move[1] - '1') << 3) | (move[0] - 'a');
    res |= src;

    // Promotion
    if (move.size() == 5) {
        res |= 0x4000;
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

    enum enumPiece piece = getPieceIndexFromSquare(res & 0x3f);

    // En passant-able flag
    if ((piece == nWhitePawn || piece == nBlackPawn) && abs((char) src - (char) dst) == 16) {
        res |= 0x8000;
    }

    // Castling Flag
    if ((piece == nWhiteKing || piece == nBlackKing) && abs((char) src - (char) dst) == 2) {
        res |= 0xc000;
    }

    return res;
}

/**
 * @brief This function converts an internal representation move encoding to a
 * SAN=0 one. See README.md and moveGen.h.
 *
 * @param uint16_t an internal encoding.
 * @return std::string the source file and rank is written on the first positions of
 * the array, respectively the destination on the last. On the last position
 * a promotion is marked.
 */
std::string Board::convertMoveToSan(uint16_t move) {
    std::string res;
    res.push_back((move & 7) + 'a');
    res.push_back(((move >> 3) & 7) + '1');
    res.push_back(((move >> 6) & 7) + 'a');
    res.push_back(((move >> 9) & 7) + '1');

    if ((move >> 14) == 1) {
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

char flagsC[19]; // Debug lines.
snprintf(flagsC, 18, "0x%lx", flags);
output += "\n" + std::string(flagsC) + "\n";

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
    const U64 enPassant = (((move >> 14) & 3) == 2);

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
    // TODO: Check if an attack happens and the src and dest are "good".
    uint16_t srcSquare = move & 0x3f;
    uint16_t destSquare = (move >> 6) & 0x3f;

    /**
     * Atentie: Trebuie negata logica!
     * Verificam daca: 1. Este setat flag-ul x en pasant-able pentru file-ul
     * destinatie.
     * 2. Este atacat rank 3 sau 7, in functie de partea care ataca.
     * 3. Piesa src == pion.
    */
    U64 enPassantFlag = ((1LL << (destSquare % 8)) << ((1 - sideToMove) << 3));
    uint8_t attackedRank = (sideToMove == Side::whiteSide ? 5 : 2);
    enum enumPiece srcPiece = getPieceIndexFromSquare(srcSquare);

logger.raw("En passant prep conditions:\n");
logger.raw(std::to_string(!(flags & enPassantFlag)) + "\n");
logger.raw(std::to_string(!(attackedRank == (destSquare / 8)))
    + " " + std::to_string(attackedRank) + " " + std::to_string((destSquare / 8)) + "\n");
logger.raw(std::to_string(!(srcPiece == (sideToMove == Side::whiteSide ? nWhitePawn : nBlackPawn))) + "\n");

    if (!(flags & enPassantFlag) || !(attackedRank == (destSquare / 8)) ||
            !(srcPiece == (sideToMove == Side::whiteSide ?
            nWhitePawn : nBlackPawn))) {
        // No en passant-able flags set. Nothing to prep.
        return;
    }

logger.raw("En passant prep is being done!\n");

    U64 destPosBoard = 1;
    destPosBoard <<= destSquare;

    /**
     * Get the current position bitBrd of the attacked pawn, colour dependent: 
     * White pawns will be one rank higher, black pawns will be one rank lower.
     * 
     * Note: flags >> ((1 - sideToMove) << 3) gets the en passant flags for the
     * side opposite of the one to attack.
     * Side::whiteSide = 0 and Side::blackSide = 1;
     * >> (destSquare % 8) gets the file of the pawn being attacked;
     * & 1 ignores the other flags;
     * << destSquare transforms the bit into a bitboard.
    */
    U64 srcPosBoard = 1LL << destSquare;
    // Shift in case of a white pawn.
    srcPosBoard <<= ((1 - sideToMove) << 3);
    // Shift in case of a black pawn.
    srcPosBoard >>= ((sideToMove) << 3);

logger.logBB(srcPosBoard);

    // Also acts as a pseudo if thanks to the trash piece optimization.
    enum enumPiece sourceSquareIndex = getPieceIndexFromSquare(
        getSquareIndex(srcPosBoard));

    // Remove pawn from its current position.
    pieceBB[sourceSquareIndex] ^= srcPosBoard;

    // Add pawn to its en passant capture position.
    pieceBB[sourceSquareIndex] |= destPosBoard;
}

// TODO: Add inline if it works.

void Board::undoEnPassantAttackPrep() {
    uint16_t move = moveHistory.top();
    uint16_t oldFlags = flagsHistory.top();

    if ((oldFlags & 0xffffLL) == 0) {
        // No en passant-able pawns existed. Nothing to undo.
        return;
    }

    uint16_t destSquare = (move >> 6) & 0x3f;
    U64 destPosBoard = 1;
    destPosBoard <<= destSquare;

    // If the following code is unclear, maybe enPassantAttackPrep() comments 
    // help.
    U64 srcPosBoard = ((( (flags >> ((1 - sideToMove) << 3))
                        >> (destSquare % 8)) & 1) << destSquare);
    srcPosBoard <<= ((1 - sideToMove) << 3);
    srcPosBoard >>= ((sideToMove) << 3);

    enum enumPiece sourceSquareIndex = takeHistory.top();
    
    // Remove pawn from its en passant capture position.
    pieceBB[sourceSquareIndex] ^= destPosBoard;

    // Add pawn to its original position.
    pieceBB[sourceSquareIndex] |= srcPosBoard;
}

// TODO: Add inline if it works.

void Board::promote(uint16_t move) {
    if ((move >> 14) != 1) {
        // Promote flag not set, nothing promote.
        return;
    }

    U8 promotion = (move >> 12) & 0x3;
    uint16_t srcSquare = (move >> 6) & 0x3f;

    enum enumPiece srcSquareIndex = getPieceIndexFromSquare(srcSquare);
    enum enumPiece destSquareIndex;

    // Get the bit board index of the piece the pawn gets promoted to.
    switch (promotion) {
    case 0:
        destSquareIndex = nWhiteRook;
        break;
    
    case 1:
        destSquareIndex = nWhiteKnight;
        break;

    case 2:
        destSquareIndex = nWhiteBishop;
        break;

    case 3:
        destSquareIndex = nWhiteQueen;
        break;

    default:
        destSquareIndex = trashPiece;
        break;
    }

    /**
     * Get the color of the piece the pawn gets promoted to.
     * Note: Side::whiteSide = 0 and Side::blackSide = 1;
     * Black pieces are always right after their white counterpart.
    */
    destSquareIndex = (enum enumPiece) (destSquareIndex + sideToMove);

    // Bitboard with only the promoted pawn.
    U64 srcPosBoard = 1LL << srcSquare;

    // Remove pawn from its board.
    pieceBB[srcSquareIndex] ^= srcPosBoard;

    // Add a new piece in its stead.
    pieceBB[destSquareIndex] |= srcPosBoard;
}

void Board::demote() {
    uint16_t move = moveHistory.top();

    if (((move >> 14) & 3) != 1) {
        // Promotion flag was not set, noting to demote.
        return;
    }

    U8 promotion = (move >> 12) & 0x3;
    uint16_t srcSquare = (move >> 6) & 0x3f;

    enum enumPiece destSquareIndex;

    // Get the bit board index of the piece the pawn gets promoted to.
    switch (promotion) {
    case 0:
        destSquareIndex = nWhiteRook;
        break;
    
    case 1:
        destSquareIndex = nWhiteKnight;
        break;

    case 2:
        destSquareIndex = nWhiteBishop;
        break;

    case 3:
        destSquareIndex = nWhiteQueen;
        break;

    default:
        destSquareIndex = trashPiece;
        break;
    }

    /**
     * Get the color of the piece the pawn gets promoted to.
     * Check promote() for more info.
    */
    destSquareIndex = (enum enumPiece) (destSquareIndex + sideToMove);

    // Bitboard with only the promoted pawn.
    U64 srcPosBoard = 1LL << srcSquare;

    // Remove the new piece.
    pieceBB[destSquareIndex] ^= srcPosBoard;

    // Add pawn back to its place.
    pieceBB[srcPosBoard] |= srcPosBoard;
}

void Board::castle(uint16_t move) {
    if (((move >> 14) & 3) != 3) {
        // Castling flag was not set, nothing to do.
        return;
    }

    U64 rookSrcPosBoard;
    U64 rookDestPosBoard;
    enum enumPiece rookIndex;

    // Get the new position of the rook based on the file the king is in.
    if (((move >> 6) & 0x3f) % 8 == 2) {
        // Queen side castle.
        rookSrcPosBoard = 0x1;
        rookDestPosBoard = 0x8;
    } else {
        // King side castle.
        rookSrcPosBoard = 0x80;
        rookDestPosBoard = 0x20;
    }

    // Get the side specific positions and piece index.
    if (sideToMove == Side::whiteSide) {
        rookIndex = nWhiteRook;
    } else {
        rookIndex = nBlackRook;
        // Shift rook positions to be on the black side.
        rookSrcPosBoard <<= 56;
        rookDestPosBoard <<= 56;
    }

    // Remove rook from its original position.
    pieceBB[rookIndex] ^= rookSrcPosBoard;

    // Add rook to its new position.
    pieceBB[rookIndex] |= rookDestPosBoard;
}

void Board::undoCastle() {
    uint16_t move = moveHistory.top();

    if (((move >> 14) & 3) != 3) {
        // Castling flag was not set, nothing to undo.
        return;
    }

    U64 rookSrcPosBoard;
    U64 rookDestPosBoard;
    enum enumPiece rookIndex;

    // Get the new position of the rook based on the file the king is in.
    if (((move >> 6) & 0x3f) % 8 == 2) {
        // Queen side castle.
        rookSrcPosBoard = 0x1;
        rookDestPosBoard = 0x8;
    } else {
        // King side castle.
        rookSrcPosBoard = 0x80;
        rookDestPosBoard = 0x20;
    }

    // Get the side specific positions and piece index.
    if (sideToMove == Side::whiteSide) {
        rookIndex = nWhiteRook;
    } else {
        rookIndex = nBlackRook;
        // Shift rook positions to be on the black side.
        rookSrcPosBoard <<= 56;
        rookDestPosBoard <<= 56;
    }

    // Remove rook from its new position.
    pieceBB[rookIndex] ^= rookDestPosBoard;

    // Add rook back to its original position.
    pieceBB[rookIndex] |= rookSrcPosBoard;
}

// TODO: Add inline if it works.

void Board::resetCastleFlags(enum enumPiece movedPieceIndex,
        U64 srcPosBitboard) {
    /**
     * Note: Check board.h to see what each bit in the flags variable
     * represents, should the following code be unclear.
    */
    switch (movedPieceIndex) {
    case nWhiteKing:
        // White cannot castle queen nor king side anymore.
        flags &= 0xfffffffffffcffff;
        break;
    
    case nBlackKing:
        // Black cannot castle queen side nor king side anymore.
        flags &= 0xfffffffffff3ffff;
        break;

    case nWhiteRook:
        // Check for side.
        if (srcPosBitboard == 0x1) {
            // White cannot castle queen side anymore.
            flags &= 0xfffffffffffeffff;
        } else if (srcPosBitboard == 0x80) {
            // White cannot castle king side anymore.
            flags &= 0xfffffffffffdffff;
        }
        break;

    case nBlackRook:
        // Check for side.
        if (srcPosBitboard == 0x100000000000000) {
            // Black cannot castle queen side anymore.
            flags &= 0xfffffffffffbffff;
        } else if (srcPosBitboard == 0x8000000000000000) {
            // Black cannot castle king side anymore.
            flags &= 0xfffffffffff7ffff;
        }
        break;

    default:
        break;
    }
}

// TODO: test function, add legality check.
// TODO: Add inline if it works.

bool Board::applyMove(uint16_t move) {
    flagsHistory.push(flags);

    enPassantAttackPrep(move);

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

    resetEnPassant();
    // Note: this function works with an internal pseudo if of sorts which may
    // or may not be faster since no jumps are made.
    setEnPassant(move);

    promote(move);

    castle(move);

    resetCastleFlags(sourceSquareIndex, sourcePosBoard);

    switchSide();

    logger.raw(toString() + '\n');

    // TODO(all) change later with move legality.
    return true;
}

// TODO: Add inline if it works.
bool Board::undoMove(void) {

    if (moveHistory.empty()) {
        return false;
    }

    switchSide();

    // Set flags to their previous state.
    DIE(flagsHistory.empty(), "Error in undoMove(): flagsHistory size!");
    flags = flagsHistory.top();

    uint16_t move = moveHistory.top();

    undoCastle();

    demote();

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

    // Remove source piece from the destination position on the source board.
    pieceBB[sourceSquareIndex] ^= destPosBoard;

    // Add destination piece back to its board.
    pieceBB[destSquareIndex] |= destPosBoard;

    // Add source piece back to its initial place on its board.
    pieceBB[sourceSquareIndex] |= sourcePosBoard;

    undoEnPassantAttackPrep();

    moveHistory.pop();
    flagsHistory.pop();
    takeHistory.pop();

    return true;
}
