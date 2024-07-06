#include "Bitboard.h"

namespace Bitboards {

    Bitboard oneAt(Square sq) {
        return ((Bitboard)1) << sq;
    }

    bool contains(Bitboard board, Square sq) {
        return board && oneAt(sq);
    }

    void initPieceMoveBoards() {
        
        for (int rank = RANK_1; rank <= RANK_8; rank++) {
            for (int file = FILE_A; file <= FILE_H; file++) {
                Square curSquare = Squares::fromRankFile(rank, file);

                ROOK_MOVES[curSquare] = RANKS[rank] ^ FILES[file];

                // see header file for how diagonals were defined
                BISHOP_MOVES[curSquare] = DIAGONALS_NW[rank + file] ^ DIAGONALS_NW[(8 - rank) + file];

                Bitboard pieceBB = oneAt(curSquare);
                // masks used to remove pieces on the edges of the board for moves that would be out of bounds
                Bitboard clearRanks12 = ~(RANKS[RANK_1] | RANKS[RANK_2]);
                Bitboard clearRanks78 = ~(RANKS[RANK_7] | RANKS[RANK_8]);
                Bitboard clearFilesAB = ~(FILES[FILE_A] | FILES[FILE_B]);
                Bitboard clearFilesGH = ~(FILES[FILE_G] | FILES[FILE_H]);
                KNIGHT_MOVES[curSquare] =
                    ((pieceBB &   clearRanks12 & ~FILES[FILE_H]) << 17) |
                    ((pieceBB &   clearRanks12 & ~FILES[FILE_A]) << 15) |
                    ((pieceBB & ~RANKS[RANK_1] &   clearFilesGH) << 10) |
                    ((pieceBB & ~RANKS[RANK_1] &   clearFilesAB) <<  6) |
                    ((pieceBB &   clearRanks78 & ~FILES[FILE_A]) >> 17) |
                    ((pieceBB &   clearRanks78 & ~FILES[FILE_H]) >> 15) |
                    ((pieceBB & ~RANKS[RANK_8] &   clearFilesAB) >> 10) |
                    ((pieceBB & ~RANKS[RANK_8] &   clearFilesGH) >> 6);

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
}
