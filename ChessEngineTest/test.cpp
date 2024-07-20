#include "pch.h"

#include "../ChessEngine/Chessboard.h"

class ChessTestEnvironment : public ::testing::Environment {
protected:
    void SetUp() override {
        Bitboards::initPieceMoveBoards();
    }
};

testing::Environment* const chesstest_env = testing::AddGlobalTestEnvironment(new ChessTestEnvironment);

TEST(MoveGeneration, NumStartingMoves) {
    Chessboard c = Chessboard();
    EXPECT_EQ(c.generateAllPseudolegalMoves(Player::WHITE).size(), 20);
    EXPECT_EQ(c.generateAllPseudolegalMoves(Player::BLACK).size(), 20);
}

TEST(MoveExecution, UndoMove) {
    Chessboard c = Chessboard();
    MoveUndoInfo m1 = c.makeMove({ SQ_E2, SQ_E4 });
    MoveUndoInfo m2 = c.makeMove({ SQ_E7, SQ_E5 });
    c.undoMove(m2);
    c.undoMove(m1);

    Chessboard c2 = Chessboard(); 
    EXPECT_EQ(c.toString(), c2.toString());
}