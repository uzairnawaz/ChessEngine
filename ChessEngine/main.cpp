
#include <iostream>

#include "ChessEngine.h"

int main()
{
    ChessEngine engine = ChessEngine();
    engine.startUCI();
    /*
    engine.board = Chessboard();

    for (int i = 0; i < 200; i++) {
        Move m = engine.search(5);
        engine.board.makeMove(m);
        std::cout << Moves::toString(m) << std::endl;
    }
    */
}
