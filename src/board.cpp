/* Copyright 2021 DucaPowr Team */
#include "./board.h"

#include <bits/stdint-uintn.h>
#include <csetjmp>
#include <string>
#include <sys/types.h>
#include <random>

#include "./constants.h"
#include "./logger.h"
#include "./utils.h"

void Board::init(void) {
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

    flags = FLAGS_INIT_VALUE;

    moveHistory  = std::stack<uint16_t>();
    takeHistory  = std::stack<enum enumPiece>();
    flagsHistory = std::stack<U64>();

    checkCount[0] = 0;
    checkCount[1] = 0;

    std::uniform_int_distribution<unsigned long long int>
                                 dist(0, UINT64_MAX);
    std::mt19937 mt(1234567);
    for (uint16_t i = 0; i < 64; i++) {
        for (uint16_t j = 0; j < 12; j++) {
            pieceHashKeys[i][j] = dist(mt);
        }
    }
    for (uint16_t i = 0; i < 20; i++) {
        flagHashKeys[i] = dist(mt);
    }
    for (uint16_t i = 0; i < 64; i++) {
        checkHashKeys[i][0] = dist(mt);
        checkHashKeys[i][1] = dist(mt);
    }

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
    return (flags & (0xffLL << (side << 3))) << 24;
}

U64 Board::getAllBB(void) {
    return getPieceBB(whiteSide) | getPieceBB(blackSide);
}

U64 Board::getEmptyBB(void) {
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
    sideToMove = otherSide(sideToMove);
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

    memset(board, emptySymbol, 64);

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


    char flagsC[26]; // Debug lines.
    snprintf(flagsC, 18, "Flags: 0x%lx", flags);
    output += "\n" + std::string(flagsC);

    return output;
}

void Board::resetEnPassant(void) {
    // Note: Side::whiteSide = 0 and Side::blackSide = 1.
    flags &= (~0xffffULL);
    // flags &= (~(0xffLL << (sideToMove << 3)));
}

void Board::setEnPassant(uint16_t move) {
    // Get whether a pawn is en passant-able. Also acts as a pseudo if.
    // Check move format in moveGen.h for more info.
    const U64 enPassant = (((move >> 14) & 3) == 2);

    /**
     * Set side specific, pawn specific flag.
     * Note: Side::whiteSide = 0 and Side::blackSide = 1;
     * (move & 0x3f) % 8) gets the file of the pawn (0 is a, 7 is h);
     * << (sideToMove << 3) shifts the flag to match the appropriate side.
    */
    flags |= (enPassant << ((move & 0x3f) & 7)) << (sideToMove << 3);
}

void Board::enPassantAttackPrep(uint16_t move) {
    uint16_t srcSquare = move & 0x3f;
    uint16_t destSquare = (move >> 6) & 0x3f;

    /**
     * Get the current position bitBrd of the attacked pawn, colour dependent:
     * White pawns will be one rank higher, black pawns will be one rank lower.
     *
     */
    U64 enPassantFlag = (1LL << (destSquare & 7)) << (otherSide(sideToMove) << 3);
    uint8_t attackedRank = (sideToMove == Side::whiteSide ? 5 : 2);
    enum enumPiece srcPiece = getPieceIndexFromSquare(srcSquare);

    if (!(flags & enPassantFlag) || !(attackedRank == (destSquare >> 3)) ||
            !(srcPiece == (sideToMove == Side::whiteSide ?
            nWhitePawn : nBlackPawn))) {
        // No en passant-able flags set. Nothing to prep.
        // If the pawn is not involved in the move, there is nothing to prep
        return;
    }

    U64 destPosBoard = 1LL << destSquare;

    U64 srcPosBoard = 1LL << (destSquare - (sideToMove == Side::whiteSide ? 8
        : -8));

    enum enumPiece sourceSquareIndex = getPieceIndexFromSquare(
        getSquareIndex(srcPosBoard));

    // Remove pawn from its current position.
    pieceBB[sourceSquareIndex] ^= srcPosBoard;

    // Add pawn to its en passant capture position.
    pieceBB[sourceSquareIndex] |= destPosBoard;
}

void Board::undoEnPassantAttackPrep(void) {
    uint16_t move = moveHistory.top();

    uint16_t srcSquare = move & 0x3f;
    uint16_t destSquare = (move >> 6) & 0x3f;

    U64 enPassantFlag = (1LL << (destSquare & 7)) << (otherSide(sideToMove) << 3);
    uint8_t attackedRank = (sideToMove == Side::whiteSide ? 5 : 2);
    enum enumPiece srcPiece = getPieceIndexFromSquare(srcSquare);

    if (!(flags & enPassantFlag) || !(attackedRank == (destSquare >> 3)) ||
            !(srcPiece == (sideToMove == Side::whiteSide ?
            nWhitePawn : nBlackPawn))) {
        // No en passant-able flags set. Nothing to unprep.
        // If the pawn is not involved in the move, there is nothing to unprep
        return;
    }

    U64 destPosBoard = 1;
    destPosBoard <<= destSquare;

    // If the following code is unclear, maybe enPassantAttackPrep() comments
    // help.
    U64 srcPosBoard = 1LL << (destSquare - (sideToMove == Side::whiteSide ? 8
        : -8));

    enum enumPiece sourceSquareIndex = takeHistory.top();

    // Remove pawn from its en passant capture position.
    pieceBB[sourceSquareIndex] ^= destPosBoard;

    // Add pawn to its original position.
    pieceBB[sourceSquareIndex] |= srcPosBoard;
}

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

void Board::demote(void) {
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
    enum enumPiece srcSquareIndex = (sideToMove == Side::whiteSide ?
        nWhitePawn : nBlackPawn);
    pieceBB[srcSquareIndex] |= srcPosBoard;
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

void Board::undoCastle(void) {
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

void Board::resetCastleFlags(enum enumPiece movedPieceIndex,
        U64 srcPosBitboard, enum enumPiece destPieceIndex,
        U64 destPosBitboard) {
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

    switch (destPieceIndex) {
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
        if (destPosBitboard == 0x1) {
            // White cannot castle queen side anymore.
            flags &= 0xfffffffffffeffff;
        } else if (destPosBitboard == 0x80) {
            // White cannot castle king side anymore.
            flags &= 0xfffffffffffdffff;
        }
        break;

    case nBlackRook:
        // Check for side.
        if (destPosBitboard == 0x100000000000000) {
            // Black cannot castle queen side anymore.
            flags &= 0xfffffffffffbffff;
        } else if (destPosBitboard == 0x8000000000000000) {
            // Black cannot castle king side anymore.
            flags &= 0xfffffffffff7ffff;
        }
        break;

    default:
        break;
    }
}

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

    resetCastleFlags(sourceSquareIndex, sourcePosBoard,
            destSquareIndex, destPosBoard);

    switchSide();

    // TODO(all) change later with move legality.
    return true;
}

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


// BOARD EVAL
int Board::eval() {
    // board : eval

    int score_mg = 0;
    int score_eg = 0;

    Side me = sideToMove;
    Side them = otherSide(sideToMove);

    // PIECES
    // mg
    score_mg += bitCount(getPawnBB(me))        * PawnValueMg;
    score_mg -= bitCount(getPawnBB(them))      * PawnValueMg;
    score_mg += bitCount(getKnightBB(me))      * KnightValueMg;
    score_mg -= bitCount(getKnightBB(them))    * KnightValueMg;
    score_mg += bitCount(getBishopBB(me))      * BishopValueMg;
    score_mg -= bitCount(getBishopBB(them))    * BishopValueMg;
    score_mg += bitCount(getRookBB(me))        * RookValueMg;
    score_mg -= bitCount(getRookBB(them))      * RookValueMg;
    score_mg += bitCount(getQueenBB(me))       * QueenValueMg;
    score_mg -= bitCount(getQueenBB(them))     * QueenValueMg;
    // eg
    score_eg += bitCount(getPawnBB(me))        * PawnValueEg;
    score_eg -= bitCount(getPawnBB(them))      * PawnValueEg;
    score_eg += bitCount(getKnightBB(me))      * KnightValueEg;
    score_eg -= bitCount(getKnightBB(them))    * KnightValueEg;
    score_eg += bitCount(getBishopBB(me))      * BishopValueEg;
    score_eg -= bitCount(getBishopBB(them))    * BishopValueEg;
    score_eg += bitCount(getRookBB(me))        * RookValueEg;
    score_eg -= bitCount(getRookBB(them))      * RookValueEg;
    score_eg += bitCount(getQueenBB(me))       * QueenValueEg;
    score_eg -= bitCount(getQueenBB(them))     * QueenValueEg;

    // PIECE POSITIONING
    
    // score += bishopPairWeight * ((bishopCount + 2) >> 2);

    // // Score high if king is near friend pieces
    // score += kingFriendsWeight * (aKingsNeighbors(getKingBB(sideToMove)) &
    //         friendPieceBB);

    // // Scoring that depends on number of checks
    // score -= (1<<(6+checkDiff*checkDiff)) + 100;

    // Move number dependent scoring
    // // Knights are more valuable at the beggining
    // score += (30 - moveHistory.size()) * 5.0 / 3 * knightCount;
    // // Bishops are more valuable at the end
    // score += (moveHistory.size() - 30) * 5.0 / 3 * bishopCount;
    
    // KING PRESENCE
    if (!bitCount(getKingBB(me)))
        return INT_MIN;
    if (!bitCount(getKingBB(them)))
        return INT_MAX;


    // PHASE CALCULATION
    uint16_t phase = TotalPhase;

    phase -= bitCount(getPawnBB(me)) * PawnPhase;
    phase -= bitCount(getPawnBB(them)) * PawnPhase;
    phase -= bitCount(getKnightBB(me)) * KnightPhase;
    phase -= bitCount(getKnightBB(them)) * KnightPhase;
    phase -= bitCount(getBishopBB(me)) * BishopPhase;
    phase -= bitCount(getBishopBB(them)) * BishopPhase;
    phase -= bitCount(getRookBB(me)) * RookPhase;
    phase -= bitCount(getRookBB(them)) * RookPhase;
    phase -= bitCount(getQueenBB(me)) * QueenPhase;
    phase -= bitCount(getQueenBB(them)) * QueenPhase;

    phase = (phase * 256 + 12) / TotalPhase;

    return ((score_mg * (256 - phase)) + (score_eg * phase)) / 256;
}

U64 Board::hash() {
    U64 hash = 0;
    for (size_t i = 0; i < 12; i++) {
        auto pieces = getSeparatedBits(pieceBB[i]);
        for (U64 pieceBB : pieces) {
            int sqIndex = getSquareIndex(pieceBB);

            hash ^= pieceHashKeys[sqIndex][i];
        }
    }
    for (size_t i = 0; i < 20; i++) {
        if (flags & (1 << i)) {
            hash ^= flagHashKeys[i];
        }
    }
    hash ^= checkHashKeys[checkCount[whiteSide]][whiteSide];
    hash ^= checkHashKeys[checkCount[blackSide]][blackSide];
    return hash;
}