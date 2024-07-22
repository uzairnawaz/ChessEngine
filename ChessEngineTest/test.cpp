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
    c.makeMove({ Square::E2, Square::E4 });
    EXPECT_EQ(c.generateAllLegalMoves().size(), 20);
}

TEST(MoveGeneration, RookMoves) {
    Chessboard c = Chessboard("k7/1r2B3/7N/7p/8/1q6/8/2K4R w - - 0 1");
    std::vector<Move> wMoves = c.generateAllLegalMoves();
    c = Chessboard("k7/1r2B3/7N/7p/8/1q6/8/2K4R b - - 0 1");
    std::vector<Move> bMoves = c.generateAllLegalMoves();
    int numWhiteRookMoves = 0;
    int numBlackRookMoves = 0;
    for (Move& m : wMoves) {
        if (m.from == Square::H1) {
            numWhiteRookMoves++;
        }
    }
    for (Move& m : bMoves) {
        if (m.from == Square::B7) {
            numBlackRookMoves++;
        }
    }
    EXPECT_EQ(numWhiteRookMoves, 8);
    EXPECT_EQ(numBlackRookMoves, 8);
}

TEST(MoveGeneration, BishopMoves) {
    Chessboard c = Chessboard("2b2k2/8/N3r3/5P2/3N3p/8/5B2/1K6 w - - 0 1");
    std::vector<Move> wMoves = c.generateAllLegalMoves();
    c = Chessboard("2b2k2/8/N3r3/5P2/3N3p/8/5B2/1K6 b - - 0 1");
    std::vector<Move> bMoves = c.generateAllLegalMoves();
    int numWhiteBishopMoves = 0;
    int numBlackBishopMoves = 0;
    for (Move& m : wMoves) {
        if (m.from == Square::F2) {
            numWhiteBishopMoves++;
        }
    }
    for (Move& m : bMoves) {
        if (m.from == Square::C8) {
            numBlackBishopMoves++;
        }
    }
    EXPECT_EQ(numWhiteBishopMoves, 5);
    EXPECT_EQ(numBlackBishopMoves, 3);
}

TEST(MoveGeneration, EnPassant) {
    Chessboard c = Chessboard("rnbqkbnr/pp1ppppp/8/1PpP4/8/8/P1P1PPPP/RNBQKBNR w KQkq c6 0 1");
    std::vector<Move> moves = c.generateAllLegalMoves();
    bool b5c6 = false;
    bool d5c6 = false;
    for (Move& m : moves) {
        if (m.to == Square::C6) {
            if (m.from == Square::B5) {
                b5c6 = true;
            }
            else if (m.from == Square::D5) {
                d5c6 = true;
            }
            else {
                FAIL();
            }
        }
    }
    EXPECT_TRUE(b5c6);
    EXPECT_TRUE(d5c6);
    c.makeMove({ Square::B5, Square::C6 });
    EXPECT_EQ(c.toString(), Chessboard("rnbqkbnr/pp1ppppp/2P5/3P4/8/8/P1P1PPPP/RNBQKBNR b KQkq - 0 1").toString());
}

TEST(MoveGeneration, PawnMoves) {
    Chessboard c = Chessboard("8/4k3/q7/1P6/2P2K1n/3b4/P2P1P1P/8 w - - 0 1");
    EXPECT_EQ(c.generateAllLegalMoves().size(), 12);
}

TEST(MoveGeneration, KingSafety) {
    Chessboard c = Chessboard("8/3k4/8/8/3q4/8/4K3/8 w - - 0 1");
    EXPECT_EQ(c.generateAllLegalMoves().size(), 3);
}

TEST(MoveGeneration, Castle) {
    Chessboard c1 = Chessboard("r3k2r/ppp1pppp/8/3p4/2n3Q1/8/PPP2PPP/R3K1R1 w Qkq - 0 1");
    std::vector<Move> wMoves = c1.generateAllLegalMoves();
    Chessboard c2 = Chessboard("r3k2r/ppp1pppp/8/3p4/2n3Q1/8/PPP2PPP/R3K1R1 b Qkq - 0 1");
    std::vector<Move> bMoves = c2.generateAllLegalMoves();
    int wCount = 0;
    int bCount = 0;
    for (Move& m : wMoves) {
        if (m.from == Square::E1) {
            switch (m.to) {
            case Square::F1:
            case Square::D1:
            case Square::E2:
            case Square::C1:
                wCount++;
                break;
            default:
                FAIL();
            }
        }
    }
    EXPECT_EQ(wCount, 4);

    for (Move& m : bMoves) {
        if (m.from == Square::E8) {
            switch (m.to) {
            case Square::D8:
            case Square::F8:
            case Square::G8:
                bCount++;
                break;
            default:
                FAIL();
            }
        }
    }
    EXPECT_EQ(bCount, 3);

    c1.makeMove({ E1, C1 });
    EXPECT_EQ(c1.toString(), Chessboard("r3k2r/ppp1pppp/8/3p4/2n3Q1/8/PPP2PPP/2KR2R1 b kq - 0 1").toString());
    c2.makeMove({ E8, G8 });
    EXPECT_EQ(c2.toString(), Chessboard("r4rk1/ppp1pppp/8/3p4/2n3Q1/8/PPP2PPP/R3K1R1 w Q - 0 1").toString());

    // test ability to castle in check
    Chessboard c3 = Chessboard("8/3k4/4q3/8/8/8/8/R3K2R w KQ - 0 1");
    EXPECT_EQ(c3.generateAllLegalMoves().size(), 4);

}

TEST(MoveGeneration, PawnPromotion) {
    Chessboard c = Chessboard("2k5/5P2/8/8/8/8/8/2K5 w - - 0 1");
    EXPECT_EQ(c.generateAllLegalMoves().size(), 9);
}

TEST(PERFT, StartingPosition) {
    Chessboard c = Chessboard();
    EXPECT_EQ(c.perft(1), 20);
    EXPECT_EQ(c.perft(2), 400);
    EXPECT_EQ(c.perft(3), 8902);
    EXPECT_EQ(c.perft(4), 197281);
    EXPECT_EQ(c.perft(5), 4865609);
}

TEST(PERFT, Kiwipete) {
    Chessboard c = Chessboard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    EXPECT_EQ(c.perft(1), 48);
    EXPECT_EQ(c.perft(2), 2039);
    EXPECT_EQ(c.perft(3), 97862);
    EXPECT_EQ(c.perft(4), 4085603);
    //EXPECT_EQ(c.perft(5), 193690690);
}

TEST(PERFT, Pos3) {
    Chessboard c = Chessboard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    EXPECT_EQ(c.perft(1), 14);
    EXPECT_EQ(c.perft(2), 191);
    EXPECT_EQ(c.perft(3), 2812);
    EXPECT_EQ(c.perft(4), 43238);
    EXPECT_EQ(c.perft(5), 674624);
    //EXPECT_EQ(c.perft(6), 11030083);
}

TEST(MoveExecution, UndoMoveNoCapture) {
    Chessboard c = Chessboard();
    MoveUndoInfo m1 = c.makeMove({ E2, E4 });
    MoveUndoInfo m2 = c.makeMove({ E7, E5 });
    c.undoMove(m2);
    c.undoMove(m1);

    Chessboard c2 = Chessboard(); 
    EXPECT_EQ(c.toString(), c2.toString());
}

TEST(MoveExecution, UndoMoveCapture) {
    Chessboard c = Chessboard("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/4PN2/PPP2PPP/RNBQKB1R b KQkq - 0 1");
    MoveUndoInfo m1 = c.makeMove({ C6, D4 });
    EXPECT_EQ(c.toString(), Chessboard("r1bqkbnr/ppp1pppp/8/3p4/3n4/4PN2/PPP2PPP/RNBQKB1R w KQkq - 0 1").toString());
    c.undoMove(m1);
    EXPECT_EQ(c.toString(), Chessboard("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/4PN2/PPP2PPP/RNBQKB1R b KQkq - 0 1").toString());
}