#include "pch.h"

#include "../ChessEngine/Chessboard.h"

class ChessTestEnvironment : public ::testing::Environment {
protected:
    void SetUp() override {
        Bitboards::initPieceMoveBoards();
    }
};

testing::Environment* const chesstest_env = testing::AddGlobalTestEnvironment(new ChessTestEnvironment);

TEST(ChessboardVerification, NumStartingMoves) {
    Chessboard c = Chessboard();
    EXPECT_EQ(c.generateAllPseudolegalMoves(true).size(), 20);
    EXPECT_EQ(c.generateAllPseudolegalMoves(false).size(), 20);
}