// ChessEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "Chessboard.h"

int main()
{

    Bitboards::initPieceMoveBoards();
    Chessboard c = Chessboard("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    c.makeMove({ C4, C5 });
    c.makeMove({ E8, C8 });
    c.makeMove({ A1, B1 });
    printf("%d\n", c.verbosePerft(1));
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
