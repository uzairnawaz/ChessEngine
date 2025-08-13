#pragma once

#include <algorithm>
#include <random>

#include "Chessboard.h"

class ChessEngine
{
private:
    std::mt19937_64 rng;

    bool debug = false;

    static const int WHITE_CHECKMATE = INT_MAX / 2;
    static const int BLACK_CHECKMATE = -(INT_MAX / 2);

    static const int pieceValues[];

    /***
     * Perform alpha beta pruning to evaluate a position to a certain depth.
     * 
     * Alpha - best explored evaluation for white
     * Beta - best explored evaluation for black
     */
    int evalAtDepth(int depth, int alpha, int beta);

    /***
     * Parses UCI commands as a vector of tokens and performs appropriate actions
     */
    void processUCICommand(std::vector<std::string>& tokens);

    /***
     * Returns a list of pseudolegal moves that are ordered using heuristics to try to 
     * increase performance of alpha beta pruning
     * 
     * Moves that are expected to be better are placed earlier in the list
     */
    std::vector<Move> generateSortedMoves();

    /***
     * Applies heuristics to determine how good a move is expected to be.
     * Returns a relative score used to compare different moves against each other
     * based on these heuristics.
     */
    int predictMoveScore(Move m);

public:
    Chessboard board;

    ChessEngine();

    /***
     * Load board from FEN string
     */
    void loadFEN(std::string fen) { board = Chessboard(fen); }

    /***
     * Communicate using UCI through stdin/stdout
     */
    void startUCI();

    /***
     * Evaluate the current position, returning an integer representing who is currently winning.
     * Positive: white winning, Negative: black winning 
     * Magnitude of return value represents how large the advantage is.
     * 
     * Factors:
     *   - Piece values of each side
     */
    int evaluate();

    /***
     * Search for the best move up to a certain depth.
     */
    Move search(int depth);
};

