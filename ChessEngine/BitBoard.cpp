
#include <intrin.h>

#include "Bitboard.h"

namespace Bitboards {

    void initPieceMoveBoards() {
        // init move arrays for sliding pieces
        generateMagics();

        Bitboard maskOuterRanks = ~(RANKS[RANK_1] | RANKS[RANK_8]);
        Bitboard maskOuterFiles = ~(FILES[FILE_A] | FILES[FILE_H]);
        for (int rank = RANK_1; rank <= RANK_8; rank++) {
            for (int file = FILE_A; file <= FILE_H; file++) {
                Square curSquare = Squares::fromRankFile(rank, file);

                ROOK_MASKS[curSquare] = (RANKS[rank] & maskOuterRanks) ^ (FILES[file] & maskOuterFiles);

                // see header file for how diagonals were defined
                BISHOP_MASKS[curSquare] = maskOuterRanks & maskOuterFiles & (DIAGONALS_NW[rank + file] ^ DIAGONALS_NW[(8 - rank) + file]);

                Bitboard pieceBB = oneAt(curSquare);
                // masks used to remove pieces on the edges of the board for moves that would be out of bounds
                Bitboard clearRanks12 = ~(RANKS[RANK_1] | RANKS[RANK_2]);
                Bitboard clearRanks78 = ~(RANKS[RANK_7] | RANKS[RANK_8]);
                Bitboard clearFilesAB = ~(FILES[FILE_A] | FILES[FILE_B]);
                Bitboard clearFilesGH = ~(FILES[FILE_G] | FILES[FILE_H]);
                KNIGHT_MOVES[curSquare] =
                    ((pieceBB & clearRanks12 & ~FILES[FILE_H]) << 17) |
                    ((pieceBB & clearRanks12 & ~FILES[FILE_A]) << 15) |
                    ((pieceBB & ~RANKS[RANK_1] & clearFilesGH) << 10) |
                    ((pieceBB & ~RANKS[RANK_1] & clearFilesAB) <<  6) |
                    ((pieceBB & clearRanks78 & ~FILES[FILE_A]) >> 17) |
                    ((pieceBB & clearRanks78 & ~FILES[FILE_H]) >> 15) |
                    ((pieceBB & ~RANKS[RANK_8] & clearFilesAB) >> 10) |
                    ((pieceBB & ~RANKS[RANK_8] & clearFilesGH) >> 6);

                KING_MOVES[curSquare] =
                    ((pieceBB & ~RANKS[RANK_1] & ~FILES[FILE_H]) << 9) |
                    ((pieceBB & ~RANKS[RANK_1] & ~FILES[FILE_A]) << 7) |
                    ((pieceBB & ~RANKS[RANK_8] & ~FILES[FILE_A]) >> 9) |
                    ((pieceBB & ~RANKS[RANK_8] & ~FILES[FILE_H]) >> 7) |
                    ((pieceBB & ~RANKS[RANK_1]) << 8) | ((pieceBB & ~RANKS[RANK_8]) >> 8) |
                    ((pieceBB & ~FILES[FILE_H]) << 1) | ((pieceBB & ~FILES[FILE_A]) >> 1);

                PAWN_MOVES_WHITE[curSquare] = (pieceBB << 8) | ((pieceBB & RANKS[RANK_2]) << 16);
                PAWN_MOVES_BLACK[curSquare] = (pieceBB >> 8) | ((pieceBB & RANKS[RANK_2]) >> 16);
                PAWN_ATTACKS_WHITE[curSquare] = ((pieceBB && ~FILES[FILE_A]) << 7) | ((pieceBB && ~FILES[FILE_H]) << 9);
                PAWN_ATTACKS_BLACK[curSquare] = ((pieceBB && ~FILES[FILE_H]) >> 7) | ((pieceBB && ~FILES[FILE_A]) >> 9);
            }
        }
    }

    Square popLSB(Bitboard& b) {
        unsigned long index;
        _BitScanForward64(&index, b); // get index of lowest order set bit
        b &= b - 1; // b - 1 will cause the LSB of b to become 0 and will alter only the lower order bits (which are set to 0 in b)
        return (Square)index;
    }

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

    Bitboard ROOK_MASKS[NUM_SQUARES];
    Bitboard BISHOP_MASKS[NUM_SQUARES];
    Bitboard KNIGHT_MOVES[NUM_SQUARES];
    Bitboard KING_MOVES[NUM_SQUARES];

    Bitboard PAWN_MOVES_WHITE[NUM_SQUARES];
    Bitboard PAWN_ATTACKS_WHITE[NUM_SQUARES];
    Bitboard PAWN_MOVES_BLACK[NUM_SQUARES];
    Bitboard PAWN_ATTACKS_BLACK[NUM_SQUARES];

    Magic ROOK_MAGICS[NUM_SQUARES];
    Magic BISHOP_MAGICS[NUM_SQUARES];
    Bitboard ROOK_MOVES[NUM_SQUARES][MAX_ROOK_ATTACK_SETS];
    Bitboard BISHOP_MOVES[NUM_SQUARES][MAX_BISHOP_ATTACK_SETS];
}
