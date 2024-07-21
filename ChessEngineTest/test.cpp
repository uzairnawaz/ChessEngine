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
    EXPECT_EQ(c.generateAllLegalMoves().size(), 20);
    c.makeMove({ Square::SQ_E2, Square::SQ_E4 });
    EXPECT_EQ(c.generateAllLegalMoves().size(), 20);
}

TEST(MoveGeneration, RookTest) {
    Chessboard c = Chessboard("k7/1r2B3/7N/7p/8/1q6/8/2K4R w - - 0 1");
    std::vector<Move> wMoves = c.generateAllLegalMoves();
    c = Chessboard("k7/1r2B3/7N/7p/8/1q6/8/2K4R b - - 0 1");
    std::vector<Move> bMoves = c.generateAllLegalMoves();
    int numWhiteRookMoves = 0;
    int numBlackRookMoves = 0;
    for (Move& m : wMoves) {
        if (m.from == Square::SQ_H1) {
            numWhiteRookMoves++;
        }
    }
    for (Move& m : bMoves) {
        if (m.from == Square::SQ_B7) {
            numBlackRookMoves++;
        }
    }
    EXPECT_EQ(numWhiteRookMoves, 8);
    EXPECT_EQ(numBlackRookMoves, 8);
}

TEST(MoveGeneration, BishopTest) {
    Chessboard c = Chessboard("2b2k2/8/N3r3/5P2/3N3p/8/5B2/1K6 w - - 0 1");
    std::vector<Move> wMoves = c.generateAllLegalMoves();
    c = Chessboard("2b2k2/8/N3r3/5P2/3N3p/8/5B2/1K6 b - - 0 1");
    std::vector<Move> bMoves = c.generateAllLegalMoves();
    int numWhiteBishopMoves = 0;
    int numBlackBishopMoves = 0;
    for (Move& m : wMoves) {
        if (m.from == Square::SQ_F2) {
            numWhiteBishopMoves++;
        }
    }
    for (Move& m : bMoves) {
        if (m.from == Square::SQ_C8) {
            numBlackBishopMoves++;
        }
    }
    EXPECT_EQ(numWhiteBishopMoves, 5);
    EXPECT_EQ(numBlackBishopMoves, 3);
}

TEST(MoveGeneration, EnPassantTest) {
    Chessboard c = Chessboard("rnbqkbnr/pp1ppppp/8/1PpP4/8/8/P1P1PPPP/RNBQKBNR w KQkq c6 0 1");
    std::vector<Move> moves = c.generateAllLegalMoves();
    bool b5c6 = false;
    bool d5c6 = false;
    for (Move& m : moves) {
        if (m.to == Square::SQ_C6) {
            if (m.from == Square::SQ_B5) {
                b5c6 = true;
            }
            else if (m.from == Square::SQ_D5) {
                d5c6 = true;
            }
            else {
                FAIL();
            }
        }
    }
    EXPECT_TRUE(b5c6);
    EXPECT_TRUE(d5c6);
    c.makeMove({ Square::SQ_B5, Square::SQ_C6 });
    EXPECT_EQ(c.toString(), Chessboard("rnbqkbnr/pp1ppppp/2P5/3P4/8/8/P1P1PPPP/RNBQKBNR b KQkq - 0 1").toString());
}

TEST(MoveExecution, UndoMoveNoCapture) {
    Chessboard c = Chessboard();
    MoveUndoInfo m1 = c.makeMove({ SQ_E2, SQ_E4 });
    MoveUndoInfo m2 = c.makeMove({ SQ_E7, SQ_E5 });
    c.undoMove(m2);
    c.undoMove(m1);

    Chessboard c2 = Chessboard(); 
    EXPECT_EQ(c.toString(), c2.toString());
}

TEST(MoveExecution, UndoMoveCapture) {
    Chessboard c = Chessboard("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/4PN2/PPP2PPP/RNBQKB1R b KQkq - 0 1");
    MoveUndoInfo m1 = c.makeMove({ SQ_C6, SQ_D4 });
    EXPECT_EQ(c.toString(), Chessboard("r1bqkbnr/ppp1pppp/8/3p4/3n4/4PN2/PPP2PPP/RNBQKB1R w KQkq - 0 1").toString());
    c.undoMove(m1);
    EXPECT_EQ(c.toString(), Chessboard("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/4PN2/PPP2PPP/RNBQKB1R b KQkq - 0 1").toString());
}