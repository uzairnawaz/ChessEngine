
#include <iostream>

#include "ChessEngine.h"

int main()
{
    ChessEngine engine = ChessEngine();
    engine.board = Chessboard();

    for (int i = 0; i < 80; i++) {
        Move m = engine.search(4);
        engine.board.makeMove(m);
        std::cout << Moves::toString(m) << std::endl;
    }
}
