#pragma once

#include <stdint.h>
#include <string>

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
const int MAX_ROOK_ATTACK_SETS = 4096;
const int MAX_BISHOP_ATTACK_SETS = 512;

enum Rank : uint8_t {
    RANK_1, RANK_2, RANK_3, RANK_4,
    RANK_5, RANK_6, RANK_7, RANK_8
};

enum File : uint8_t {
    FILE_A, FILE_B, FILE_C, FILE_D,
    FILE_E, FILE_F, FILE_G, FILE_H
};

enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQUARE_NONE
};

namespace Squares {
    inline Square fromRankFile(int r, int f) { return (Square)(r * 8 + f); };
    inline Square fromAlgebraic(const char algebraic[2]) { return fromRankFile(algebraic[1] - '1', algebraic[0] - 'a'); }
    inline Rank getRank(Square s) { return (Rank)(s / 8); }
    inline File getFile(Square s) { return (File)(s % 8); }
    inline std::string toAlgebraic(Square s) { return std::string({ (char)('a' + getFile(s)), (char)('1' + getRank(s)) }); }
}

namespace Bitboards {

    /***
     * Create a bitboard that contains a single 1 at the specified coordinate.
     * The rest of the board is filled with 0s.
     */
    inline Bitboard oneAt(Square s) { return s == Square::SQUARE_NONE ? 0 : ((Bitboard)1) << s; };

    /***
     * Check if a piece is present at the given x and y coordinates.
     * X and Y are 0-indexed, starting from the bottom left corner of the board.
     */
    inline bool contains(Bitboard b, Square s) { return b & oneAt(s); }

    /***
     * Pops the least significant bit from a given bitboard.
     * Returns the square that the popped bit was found at.
     */
    Square popLSB(Bitboard& b);

    /***
     * Initializes move bitboards for each piece type
     */
    void initPieceMoveBoards();

    /***
     * Generates a table of magic bitboards for sliding pieces
     */
    void generateMagics();

    /***
     * Returns the bitboard containing the moves for a rook given a set of blockers.
     */
    Bitboard getRookMoveTable(Square sq, Bitboard blockers);

    /***
     * Returns the bitboard containing the moves for a bishop given a set of blockers.
     */
    Bitboard getBishopMoveTable(Square sq, Bitboard blockers);

    extern const Bitboard RANKS[];
    extern const Bitboard FILES[];

    /*
     * Defined so that rank + file of a square gives you its index in this array
     * Ex: b3 
     *     rank: 2 (0-indexed), file: 1  --> 1 + 2 = 3
     * DIAGONALS_NW[3] will contain the bitboard of the diagonal that crosses through b3
     */
    extern const Bitboard DIAGONALS_NW[];

    /*
     * Defined so that (7 - rank) + file of a square gives you its index in this array
     * Ex: b3
     *     rank: 2 (0-indexed), file: 1  --> (7 - 2) + 1 = 6
     * DIAGONALS_NE[6] will contain the bitboard of the diagonal that crosses through b3
     */
    extern const Bitboard DIAGONALS_NE[];

    /*
     * Masks to check if any pieces are between the king and rook
     * Used to see if there is a clear path for castling
     */
    extern const Bitboard WHITE_KINGSIDE;
    extern const Bitboard WHITE_QUEENSIDE;
    extern const Bitboard BLACK_KINGSIDE;
    extern const Bitboard BLACK_QUEENSIDE;

    extern Bitboard ROOK_MASKS[NUM_SQUARES];
    extern Bitboard BISHOP_MASKS[NUM_SQUARES];

    extern Bitboard KNIGHT_MOVES[NUM_SQUARES];
    extern Bitboard KING_MOVES[NUM_SQUARES];

    extern Bitboard PAWN_MOVES_WHITE[NUM_SQUARES];
    extern Bitboard PAWN_ATTACKS_WHITE[NUM_SQUARES];
    extern Bitboard PAWN_MOVES_BLACK[NUM_SQUARES];
    extern Bitboard PAWN_ATTACKS_BLACK[NUM_SQUARES];

    struct Magic {
        Bitboard magic;
        int shift;
    };
    extern Magic ROOK_MAGICS[NUM_SQUARES];
    extern Magic BISHOP_MAGICS[NUM_SQUARES];

    extern Bitboard ROOK_MOVES[NUM_SQUARES][MAX_ROOK_ATTACK_SETS];
    extern Bitboard BISHOP_MOVES[NUM_SQUARES][MAX_BISHOP_ATTACK_SETS];
}
