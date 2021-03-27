/* Copyright 2021 DucaPowr Team */
#pragma once

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

