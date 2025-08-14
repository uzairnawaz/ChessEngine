#pragma once

#include <algorithm>
#include <iostream>
#include <sstream>
#include <random>

#include "Chessboard.h"

static void print(std::string s) {
    std::cout << s.c_str() << std::endl;
}

template <typename EvalFunction = int(*)(const Chessboard&)>
class ChessEngine
{
private:
    bool debug = false;

    static constexpr int WHITE_CHECKMATE = INT_MAX / 2;
    static constexpr int BLACK_CHECKMATE = -(INT_MAX / 2);

    static constexpr int pieceValues[] = {
       100,             // pawn
       300,             // knight
       300,             // bishop
       500,             // rook
       900,             // queen
       WHITE_CHECKMATE  // king
    };

    /***
     * Perform alpha beta pruning to evaluate a position to a certain depth.
     * 
     * Alpha - best explored evaluation for white
     * Beta - best explored evaluation for black
     */
    int evalAtDepth(int depth, int alpha, int beta) {
        if (depth == 0) {
            return evaluate(board);
        }

        std::vector<Move> moves = generateSortedMoves();
        if (moves.size() == 0) {
            if (board.isChecked(board.getTurn())) {
                if (board.getTurn() == Player::WHITE) {
                    return BLACK_CHECKMATE;
                }
                return WHITE_CHECKMATE;
            }
            return 0;
        }

        int best = board.getTurn() == Player::WHITE ? INT_MIN : INT_MAX; // initialize to worst case
        int numMovesTested = 0;
        for (Move& move : moves) {
            MoveUndoInfo moveInfo = board.makeMove(move);
            int eval = evalAtDepth(depth - 1, alpha, beta);
            if (board.getTurn() == Player::BLACK) { // white just moved
                // we want to maximize eval function
                best = best > eval ? best : eval;
                if (best > beta) {
                    board.undoMove(moveInfo);
                    return best;
                }
                alpha = best > alpha ? best : alpha;
            }
            else { // black just moved
                // we want to minimize eval function
                best = best < eval ? best : eval;
                if (best < alpha) {
                    board.undoMove(moveInfo);
                    return best;
                }
                beta = best < beta ? best : beta;
            }
            board.undoMove(moveInfo);
        }

        return best;
    }

    /***
     * Parses UCI commands as a vector of tokens and performs appropriate actions
     */
    void processUCICommand(std::vector<std::string>& tokens) {
        if (tokens[0] == "uci") {
            print("id name SuperCoolEngine");
            print("id author Uzair Nawaz");

            print("uciok");
        }
        else if (tokens[0] == "debug") {
            debug = tokens[1] == "on";
        }
        else if (tokens[0] == "isready") {
            print("readyok");
        }
        else if (tokens[0] == "setoption") {

        }
        else if (tokens[0] == "register") {

        }
        else if (tokens[0] == "ucinewgame") {
            board = Chessboard();
        }
        else if (tokens[0] == "position") {
            int movesToken = 0;
            for (int i = 0; i < tokens.size(); i++) {
                if (tokens[i] == "moves") {
                    movesToken = i;
                    break;
                }
            }

            if (tokens[1] == "startpos") {
                board = Chessboard();
            }
            else if (tokens[1] == "fen") {
                std::string fen = "";
                int fenEnd = movesToken == 0 ? tokens.size() : movesToken;
                for (int i = 1; i < fenEnd; i++) {
                    fen += tokens[i];
                }
                board = Chessboard(fen);
            }

            if (movesToken != 0) {
                for (int i = movesToken + 1; i < tokens.size(); i++) {
                    Move m = { Squares::fromAlgebraic(tokens[i].substr(0, 2).c_str()), Squares::fromAlgebraic(tokens[i].substr(2, 2).c_str()) };
                    if (tokens[i].size() == 5) {
                        switch (tokens[i][5]) {
                        case 'p':
                            m.promotion = Piece::PAWN;
                            break;
                        case 'n':
                            m.promotion = Piece::KNIGHT;
                            break;
                        case 'b':
                            m.promotion = Piece::BISHOP;
                            break;
                        case 'r':
                            m.promotion = Piece::ROOK;
                            break;
                        case 'q':
                            m.promotion = Piece::QUEEN;
                            break;
                        }
                    }
                    board.makeMove(m);
                }
            }

        }
        else if (tokens[0] == "go") {
            Move m = search(5);
            print("bestmove " + Moves::toString(m));
            if (tokens[1] == "searchmoves") {

            }
            else if (tokens[1] == "ponder") {

            }
            else if (tokens[1] == "wtime") {

            }
            else if (tokens[1] == "btime") {

            }
            else if (tokens[1] == "winc") {

            }
            else if (tokens[1] == "binc") {

            }
            else if (tokens[1] == "movestogo") {

            }
            else if (tokens[1] == "depth") {

            }
            else if (tokens[1] == "nodes") {

            }
            else if (tokens[1] == "mate") {

            }
            else if (tokens[1] == "movetime") {

            }
            else if (tokens[1] == "infinite") {

            }
        }
        else if (tokens[0] == "stop") {

        }
        else if (tokens[0] == "ponderhit") {

        }
        else if (tokens[0] == "quit") {
            exit(1);
        }
    }

    /***
     * Returns a list of pseudolegal moves that are ordered using heuristics to try to 
     * increase performance of alpha beta pruning
     * 
     * Moves that are expected to be better are placed earlier in the list
     */
    std::vector<Move> generateSortedMoves() {
        std::vector<Move> moves = board.generateAllLegalMoves();
        std::sort(moves.begin(), moves.end(),
            [this](Move& m1, Move& m2) {
                return predictMoveScore(m1) < predictMoveScore(m2);
            }
        );
        return moves;
    }

    /***
     * Applies heuristics to determine how good a move is expected to be.
     * Returns a relative score used to compare different moves against each other
     * based on these heuristics
     */
    int predictMoveScore(Move m) {
        int score = 0;

        Piece fromPiece = board.getPieceTypeAtSquareGivenColor(m.from, board.getTurn());
        Piece toPiece = board.getPieceTypeAtSquareGivenColor(m.to, Players::getEnemy(board.getTurn()));

        // prioritize capturing high value pieces with low value pieces
        if (toPiece != Piece::PIECE_NONE) {
            score += 10 * pieceValues[toPiece] - pieceValues[fromPiece];
        }

        // incentivize pawn promotion
        if (m.promotion != Piece::PIECE_NONE) {
            score += pieceValues[m.promotion];
        }

        return score;
    }

    /***
     * Evaluate the current position, returning an integer representing who is currently winning.
     * Positive: white winning, Negative: black winning
     * Magnitude of return value represents how large the advantage is.
     *
     * Factors:
     *   - Piece values of each side
     */
    static int evaluatePieceValue(const Chessboard& board) {
        // if its our turn and the enemy king is already under attack, we win!
        // this is used to prevent choosing illegal moves
        if (board.isChecked(Players::getEnemy(board.getTurn()))) {
            return board.getTurn() == Player::WHITE ? WHITE_CHECKMATE : BLACK_CHECKMATE;
        }

        int eval = 0;

        // evaluate based on piece values:
        for (int p = Piece::PAWN; p <= Piece::KING; p++) {
            eval += board.countPieces(Player::WHITE, (Piece)p) * pieceValues[p];
            eval -= board.countPieces(Player::BLACK, (Piece)p) * pieceValues[p];
        }

        // add a little bit of randomness just to make moves more interesting when 
        // there is no piece value differences
        static std::mt19937_64 rng;
        std::uniform_int_distribution<int> dist(-5, 5);
        eval += dist(rng);

        return eval;
    }

public:
    Chessboard board;
    EvalFunction evaluate;

    ChessEngine() : evaluate(&ChessEngine::evaluatePieceValue) {
        Bitboards::initPieceMoveBoards();
    }

    ChessEngine(EvalFunction eval): evaluate(eval) {
        Bitboards::initPieceMoveBoards();
    }

    /***
     * Load board from FEN string
     */
    void loadFEN(std::string fen) { board = Chessboard(fen); }

    /***
     * Communicate using UCI through stdin/stdout
     */
    void startUCI() {
        while (true) {
            std::string command;
            std::getline(std::cin, command);
            std::stringstream commandStream(command);
            std::string token;
            std::vector<std::string> tokens;
            while (commandStream >> token) {
                tokens.push_back(token);
            }

            processUCICommand(tokens);
        }
    }

    /***
     * Search for the best move up to a certain depth.
     */
    Move search(int depth) {
        std::vector<Move> moves = generateSortedMoves();
        int bestEval = board.getTurn() == Player::WHITE ? INT_MIN : INT_MAX; // initialize to worst case
        Move bestMove = moves[0];

        for (Move& move : moves) {
            MoveUndoInfo moveInfo = board.makeMove(move);
            int eval = evalAtDepth(depth - 1, INT_MIN, INT_MAX);
            if ((board.getTurn() == Player::BLACK && eval > bestEval) || // was white turn
                (board.getTurn() == Player::WHITE && eval < bestEval)) { // was black turn
                bestEval = eval;
                bestMove = move;
            }
            board.undoMove(moveInfo);
        }

        return bestMove;
    }
};

