#pragma once

#include <string>
#include <stdio.h>
#include <vector>

#include "Bitboard.h"

/***
 * Helper struct containing the castling permissions of each side
 * ex: 
 *  wKingside - true if white can castle kingside
 */
struct CastleAbility {
    bool wKingside = false;
    bool wQueenside = false;
    bool bKingside = false;
    bool bQueenside = false;
};

enum Player {
    WHITE = 0,
    BLACK = 6
};

namespace Players {
    inline Player getEnemy(Player p) { return (Player)(6 - p); }
}

enum Piece {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    PIECE_NONE
};

struct Move {
    Square from;
    Square to;
    Piece promotion = Piece::PIECE_NONE;
};

struct MoveUndoInfo {
    Move move;
    Piece captured;
    CastleAbility castleAbility;
    Square enPassantTarget;
};

class Chessboard
{
private:
    Bitboard pieces[12] = { }; // bitboards for each piece type and color (6 white piece boards, 6 black piece boards)
                               // board index accessible by performing Piece + Player (ex pieces[PAWN + WHITE] = white pawns bitboard)

    Player currentTurn;
    CastleAbility castleAbility;
    Square enPassantTarget;
    int halfMoveClock;
    int fullMoveNumber;

    /***
     * The following functions generate all pseudo legal moves for the given piece type and store
     * the generated moves in outMoveArray.
     */
    void generatePawnMoves(std::vector<Move>& outMoveArray);
    void generateKnightMoves(std::vector<Move>& outMoveArray);
    void generateBishopMoves(std::vector<Move>& outMoveArray);
    void generateRookMoves(std::vector<Move>& outMoveArray);
    void generateQueenMoves(std::vector<Move>& outMoveArray);
    void generateKingMoves(std::vector<Move>& outMoveArray);

    /***
     * Return a bitboard containing all of the pieces on the board.
     */
    Bitboard getAllPieces() { return getAllPiecesByColor(Player::WHITE) | getAllPiecesByColor(Player::BLACK); }

    /***
     * Return a bitboard containing all of the pieces of a certain color
     */
    Bitboard getAllPiecesByColor(Player color);

    /***
     * Get the type of a piece at a given square given that it is of a
     * certain color
     */
    Piece getPieceTypeAtSquareGivenColor(Square s, Player player);

    /***
     * Return true if a given player is attacking a specified square.
     */
    bool isAttacking(Player player, Square sq);

    /***
     * Return true if a given player is under check
     */
    bool isChecked(Player p);

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
     * Return a string representation of the board, used for debugging
     */
    std::string toString();

    /***
     * Generate all pseudolegal moves for the current position for the current player
     */
    std::vector<Move> generateAllPseudolegalMoves();

    /***
     * Generate all legal moves for the current position
     */
    std::vector<Move> generateAllLegalMoves();

    /***
     * Performs a given legal move on the board.
     * Returns a struct containing information necessary to undo the move.
     */
    MoveUndoInfo makeMove(Move m);

    /***
     * Undo a move on the board, given information about the move.
     */
    void undoMove(MoveUndoInfo m);

    /***
     * Counts the number of legal moves at a certain depth.
     * Used for debugging purposes.
     */
    unsigned long perft(int depth);
    unsigned long verbosePerft(int depth);
};

