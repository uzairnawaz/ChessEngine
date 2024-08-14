
#include <iostream>

#include "ChessEngine.h"

int main()
{
    ChessEngine engine = ChessEngine();
    engine.board = Chessboard("r1bqkb1r/ppp2p2/2np1n1p/4p1p1/P1B1P3/2PP1N2/1P3PPP/RNBQK2R w KQkq g6 0 7");

    for (int i = 0; i < 200; i++) {
        Move m = engine.search(5);
        engine.board.makeMove(m);
        std::cout << Moves::toString(m) << std::endl;
    }
}
