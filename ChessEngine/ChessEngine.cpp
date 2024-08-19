
#include <iostream>
#include <sstream>
#include <random>

#include "ChessEngine.h"

const int ChessEngine::pieceValues[] = {
       100,             // pawn
       300,             // knight
       300,             // bishop
       500,             // rook
       900,             // queen
       WHITE_CHECKMATE  // king
};

ChessEngine::ChessEngine() {
    Bitboards::initPieceMoveBoards();
}

int ChessEngine::evaluate() {
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
    std::uniform_int_distribution<int> dist(-5, 5);
    eval += dist(rng);

    return eval;
}

Move ChessEngine::search(int depth) {
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

int ChessEngine::evalAtDepth(int depth, int alpha, int beta) {
    if (depth == 0) {
        return evaluate();
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

std::vector<Move> ChessEngine::generateSortedMoves() {
    std::vector<Move> moves = board.generateAllLegalMoves();
    std::sort(moves.begin(), moves.end(), 
        [this](Move& m1, Move& m2) {
            return predictMoveScore(m1) < predictMoveScore(m2);
        }
    );
    return moves;
}

int ChessEngine::predictMoveScore(Move m) {
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

void ChessEngine::startUCI() {
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

void print(std::string s) {
    std::cout << s.c_str() << std::endl;
}

void ChessEngine::processUCICommand(std::vector<std::string>& tokens) {
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

