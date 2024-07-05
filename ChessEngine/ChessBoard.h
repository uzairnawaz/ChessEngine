#pragma once

#include <string>

#include "Bitboard.h"

/***
 * Helper struct containing the castling permissions of each side
 * ex: 
 *  wKingside - true if white can castle kingside
 */
typedef struct CastleAbility {
    bool wKingside = false;
    bool wQueenside = false;
    bool bKingside = false;
    bool bQueenside = false;
};

class Chessboard
{
private:
    Bitboard whitePawns = 0;
    Bitboard whiteKnights = 0;
    Bitboard whiteBishops = 0;
    Bitboard whiteRooks = 0;
    Bitboard whiteQueens = 0;
    Bitboard whiteKings = 0;
    Bitboard blackPawns = 0;
    Bitboard blackKnights = 0;
    Bitboard blackBishops = 0;
    Bitboard blackRooks = 0;
    Bitboard blackQueens = 0;
    Bitboard blackKings = 0;

    bool isWhiteTurn;
    CastleAbility castleAbility;
    Square enPessantTarget;
    int halfMoveClock;
    int fullMoveNumber;

public:
    /***
     * Initialize a chess board with the normal starting position
     */
    Chessboard() : Chessboard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {};

    /***
     * Load a chess game state from a FEN string
     */
    Chessboard(std::string fen);

};

