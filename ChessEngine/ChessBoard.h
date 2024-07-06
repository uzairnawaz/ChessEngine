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

typedef struct Move {
    Square from;
    Square to;
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

    /***
     * Generate all pseudolegal moves for the current position
     */
    Move* generateAllPseudolegalMoves();

    /***
     * The following functions generate all pseudo legal moves for the given piece type and store
     * the generated moves in outMoveArray.
     */
    void generatePawnMoves(Move* outMoveArray);
    void generateKnightMoves(Move* outMoveArray);
    void generateBishopMoves(Move* outMoveArray);
    void generateRookMoves(Move* outMoveArray);
    void generateQueenMoves(Move* outMoveArray);
    void generateKingMoves(Move* outMoveArray);

public:
    /***
     * Initialize a chess board with the normal starting position
     */
    Chessboard() : Chessboard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {};

    /***
     * Load a chess game state from a FEN string
     */
    Chessboard(std::string fen);

    /***
     * Generate all legal moves for the current position
     */
    //Moves* generateAllLegalMoves();
};

