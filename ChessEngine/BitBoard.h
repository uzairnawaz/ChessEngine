#pragma once

#include <stdint.h>

typedef uint64_t Bitboard;

/***
 * STANDARD POSITION ORDER
 * A1 = 0
 * H8 = 63
 *
 * 56 57 58 59 60 61 62 63
 * 48 49 50 51 52 53 54 55
 * 40 41 42 43 44 45 46 47
 * 32 33 34 35 36 37 38 39
 * 24 25 26 27 28 29 30 31
 * 16 17 18 19 20 21 22 23
 *  8  9 10 11 12 13 14 15
 *  0  1  2  3  4  5  6  7
 *
 */

const int NUM_SQUARES = 64;

enum Square: uint8_t {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE
};

namespace Squares {
    Square fromRankFile(Rank r, File f) { return (Square)(r * 8 + f); };
    Square fromAlgebraic(const char algebraic[2]) { return fromRankFile(algebraic[1] - '1', algebraic[0] - 'a'); }
}

enum Rank : uint8_t {
    RANK_1, RANK_2, RANK_3, RANK_4,
    RANK_5, RANK_6, RANK_7, RANK_8
};

enum File : uint8_t {
    FILE_A, FILE_B, FILE_C, FILE_D,
    FILE_E, FILE_F, FILE_G, FILE_H
};

namespace Bitboards {

    /***
     * Create a bitboard that contains a single 1 at the specified coordinate.
     * The rest of the board is filled with 0s.
     */
    Bitboard oneAt(Square s);

    /***
     * Check if a piece is present at the given x and y coordinates.
     * X and Y are 0-indexed, starting from the bottom left corner of the board.
     */
    bool contains(Bitboard b, Square s);

    const Bitboard RANKS[] = {
        0x00000000000000ff,
        0x000000000000ff00,
        0x0000000000ff0000,
        0x00000000ff000000,
        0x000000ff00000000,
        0x0000ff0000000000,
        0x00ff000000000000,
        0xff00000000000000,
    };

    const Bitboard FILES[] = {
        0x0101010101010101,
        0x0202020202020202,
        0x0404040404040404,
        0x0808080808080808,
        0x1010101010101010,
        0x2020202020202020,
        0x4040404040404040,
        0x8080808080808080,
    };

    /*
     * Defined so that rank + file of a square gives you its index in this array
     * Ex: b3 
     *     rank: 2 (0-indexed), file: 1  --> 1 + 2 = 3
     * DIAGONALS_NW[3] will contain the bitboard of the diagonal that crosses through b3
     */
    const Bitboard DIAGONALS_NW[] = {
        0x0000000000000001,
        0x0000000000000102,
        0x0000000000010204,
        0x0000000001020408,
        0x0000000102040810,
        0x0000010204081020,
        0x0001020408102040,
        0x0102040810204080,
        0x0204081020408000,
        0x0408102040800000,
        0x0810204080000000,
        0x1020408000000000,
        0x2040800000000000,
        0x4080000000000000,
        0x8000000000000000
    };

    /*
     * Defined so that (8 - rank) + file of a square gives you its index in this array
     * Ex: b3
     *     rank: 2 (0-indexed), file: 1  --> (8 - 2) + 1 = 7
     * DIAGONALS_NE[7] will contain the bitboard of the diagonal that crosses through b3
     */
    const Bitboard DIAGONALS_NE[] = {
        0x0100000000000000,
        0x0201000000000000,
        0x0402010000000000,
        0x0804020100000000,
        0x1008040201000000,
        0x2010080402010000,
        0x4020100804020100,
        0x8040201008040201,
        0x0080402010080402,
        0x0000804020100804,
        0x0000008040201008,
        0x0000000080402010,
        0x0000000000804020,
        0x0000000000008040,
        0x0000000000000080
    };

    Bitboard ROOK_MOVES[NUM_SQUARES];
    Bitboard BISHOP_MOVES[NUM_SQUARES];
    Bitboard KNIGHT_MOVES[NUM_SQUARES];
    Bitboard KING_MOVES[NUM_SQUARES];

    Bitboard PAWN_MOVES_WHITE[NUM_SQUARES];
    Bitboard PAWN_ATTACKS_WHITE[NUM_SQUARES];
    Bitboard PAWN_MOVES_BLACK[NUM_SQUARES];
    Bitboard PAWN_ATTACKS_BLACK[NUM_SQUARES];

    /***
     * Initializes move bitboards for each piece type
     */
    void initPieceMoveBoards();

}
