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
    Square fromAlgebraic(const char algebraic[2]) { return (Square)((algebraic[0] - 'a') + 8 * (algebraic[1] - '1')); }
}

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


}
