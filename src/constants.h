/* Copyright 2021 DucaPowr Team */
#pragma once

#include "./utils.h"

// ENGINE ---------------------------------------------------------
#define QUOETS_FILE "quotes"

// XBOARD ---------------------------------------------------------
#define FEATURE_ARGS "sigint=0 san=0 name=DucaPowr colors=0 usermove=1 done=1"

// BITBOARDS ------------------------------------------------------

#define WHITEPIECESTART  0x000000000000FFFF
#define BLACKPIECESTART  0xFFFF000000000000

#define WHITEPAWNSTART   0x000000000000FF00
#define WHITEROOKSTART   0x0000000000000081
#define WHITEKNIGHTSTART 0x0000000000000042
#define WHITEBISHOPSTART 0x0000000000000024
#define WHITEQUEENSTART  0x0000000000000008
#define WHITEKINGSTART   0x0000000000000010

#define BLACKPAWNSTART   0x00FF000000000000
#define BLACKROOKSTART   0x8100000000000000
#define BLACKKNIGHTSTART 0x2400000000000000
#define BLACKBISHOPSTART 0x4200000000000000
#define BLACKQUEENSTART  0x0800000000000000
#define BLACKKINGSTART   0x1000000000000000

#define AFILE            0x0101010101010101
#define BFILE            0x0202020202020202
#define CFILE            0x0404040404040404
#define DFILE            0x0808080808080808
#define EFILE            0x1010101010101010
#define FFILE            0x2020202020202020
#define GFILE            0x4040404040404040
#define HFILE            0x8080808080808080

#define RANK1            0x00000000000000FF
#define RANK2            0x000000000000FF00
#define RANK3            0x0000000000FF0000
#define RANK4            0x00000000FF000000
#define RANK5            0x000000FF00000000
#define RANK6            0x0000FF0000000000
#define RANK7            0x00FF000000000000
#define RANK8            0xFF00000000000000

// MOVE GENERATION ------------------------------------------------
#define MAX_MOVES_AT_STEP               400

/* Bitboard * MAGICFAR8 will map file A to rank 8
 * The other files of Bitboard must be empty 
 */
#define MAGICFAR8       0x0102040810204080

// Bishop relevant occupancy bits
const int bishopRelevantBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

/*
 * Took them from:
 * https://github.com/maksimKorzh/chess_programming/blob/master/src/magics/magics.txt
 */
const U64 bishopMagics[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL,
};

