#include "Chessboard.h"

Chessboard::Chessboard(std::string fen) {
    /***
     * FEN FORMAT
     * The following pieces of information are separated by spaces:
     *   Board layout
     *   Current turn
     *   Castling availability
     *   En passant target square
     *   Halfmove clock
     *   Fullmove number
     */

     // load board position into bitboards
    int idx = 0;
    Square square = SQ_A8;
    while (fen.at(idx) != ' ') {
        switch (fen.at(idx)) {
        case 'p':
            blackPawns |= Bitboards::oneAt(square);
            break;
        case 'P':
            whitePawns |= Bitboards::oneAt(square);
            break;
        case 'n':
            blackKnights |= Bitboards::oneAt(square);
            break;
        case 'N':
            whiteKnights |= Bitboards::oneAt(square);
            break;
        case 'b':
            blackBishops |= Bitboards::oneAt(square);
            break;
        case 'B':
            whiteBishops |= Bitboards::oneAt(square);
            break;
        case 'r':
            blackRooks |= Bitboards::oneAt(square);
            break;
        case 'R':
            whiteRooks |= Bitboards::oneAt(square);
            break;
        case 'q':
            blackQueens |= Bitboards::oneAt(square);
            break;
        case 'Q':
            whiteQueens |= Bitboards::oneAt(square);
            break;
        case 'k':
            blackKings |= Bitboards::oneAt(square);
            break;
        case 'K':
            whiteKings |= Bitboards::oneAt(square);
            break;
        case '/':
            /*
            * Go to next rank
            * Example situation:
            * square = 64
            * 56 57 58 59 60 61 62 63
            * 48 49 50 51 52 53 54 55
            * 
            * Need to subtract 16 to get to the start of the next rank (pos 48)
            * Since we will be adding one after this switch statement, subtract 17 instead.
            */
            square = (Square)(square - 17);
            break;
        default:
            // should be a number 1-8 representing number of empty squares to skip
            // skip 1 less than the # of empty squares since we will advance to the next square anyway
            square = (Square)(square + (fen.at(idx) - '1'));
            break;
        }
        square = (Square)(square + 1); // go to the next file
        idx++;
    }

    idx++; // skip space
    // Load current turn
    isWhiteTurn = fen.at(idx) == 'w';

    idx += 2; // skip turn and space
    // Load castling ability
    if (fen.at(idx) != '-') {
        while (fen.at(idx) != ' ') {
            switch (fen.at(idx)) {
            case 'q':
                castleAbility.bQueenside = true;
                break;
            case 'Q':
                castleAbility.wQueenside = true;
                break;
            case 'k':
                castleAbility.bKingside = true;
                break;
            case 'K':
                castleAbility.wKingside = true;
                break;
            }
            idx++;
        }
    }
    else {
        idx++; // skip past the '-'
    }

    idx++; // skip space
    // Load en passant target square
    if (fen.at(idx) != '-') {
        enPessantTarget = Squares::fromAlgebraic(fen.substr(idx, 2).c_str());
        idx += 3; // skip past coordinate and space
    }
    else {
        enPessantTarget = SQ_NONE;
        idx += 2; // skip past '- '
    }

    // Load half move clock and full move number
    std::string halfMoveClockAndNumMoves = fen.substr(idx);
    size_t spaceIdx = halfMoveClockAndNumMoves.find(' ');
    halfMoveClock = std::stoi(halfMoveClockAndNumMoves.substr(0, spaceIdx));
    fullMoveNumber = std::stoi(halfMoveClockAndNumMoves.substr(spaceIdx));
}

Bitboard Chessboard::getAllPiecesByColor(bool isWhite) {
    if (isWhite) {
        return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKings;
    }
    return blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKings;
}

std::vector<Move>& Chessboard::generateAllPseudolegalMoves(bool isWTurn) {
    std::vector<Move>* moves = new std::vector<Move>();
    generatePawnMoves(*moves, isWTurn);
    generateKnightMoves(*moves, isWTurn);
    generateKingMoves(*moves, isWTurn);
    generateBishopMoves(*moves, isWTurn);
    generateRookMoves(*moves, isWTurn);
    generateQueenMoves(*moves, isWTurn);
    return *moves;
}

void Chessboard::generatePawnMoves(std::vector<Move>& moves, bool isWTurn) {
    Bitboard pawns = isWTurn ? whitePawns : blackPawns;
    Bitboard maskAllPieces = ~getAllPieces();
    Bitboard enemyPieces = getAllPiecesByColor(!isWTurn);
    while (pawns) {
        Square from = Bitboards::popLSB(pawns);
        Bitboard movesBoard = isWTurn ? Bitboards::PAWN_MOVES_WHITE[from] : Bitboards::PAWN_MOVES_BLACK[from];
        movesBoard = movesBoard & maskAllPieces;
        Bitboard attacksBoard = isWTurn ? Bitboards::PAWN_ATTACKS_WHITE[from] : Bitboards::PAWN_ATTACKS_BLACK[from];
        attacksBoard = attacksBoard & enemyPieces;
        while (movesBoard) {
            Square to = Bitboards::popLSB(movesBoard);
            moves.push_back({ from, to }); 
        }
        while (attacksBoard) {
            Square to = Bitboards::popLSB(attacksBoard);
            moves.push_back({ from, to });
        }
    }
}

void Chessboard::generateKnightMoves(std::vector<Move>& moves, bool isWTurn) {
    Bitboard knights = isWTurn ? whiteKnights : blackKnights;
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(isWTurn);
    while (knights) {
        Square from = Bitboards::popLSB(knights);
        Bitboard movesBoard = Bitboards::KNIGHT_MOVES[from] & maskFriendlyPieces;
        while (movesBoard) {
            Square to = Bitboards::popLSB(movesBoard);
            moves.push_back({ from, to });
        }
    }
}

void Chessboard::generateKingMoves(std::vector<Move>& moves, bool isWTurn) {
    Bitboard king = isWTurn ? whiteKings : blackKings;
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(isWTurn);
    Square from = Bitboards::popLSB(king);
    Bitboard movesBoard = Bitboards::KING_MOVES[from] & maskFriendlyPieces;
    while (movesBoard) {
        Square to = Bitboards::popLSB(movesBoard);
        moves.push_back({ from, to });
    }
}

void Chessboard::generateBishopMoves(std::vector<Move>& moves, bool isWTurn) {
    Bitboard bishops = isWTurn ? whiteBishops : blackBishops;
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(isWTurn);
    Bitboard allPieces = getAllPieces();
    while (bishops) {
        Square from = Bitboards::popLSB(bishops);
        Bitboard movesBoard = Bitboards::getBishopMoveTable(from, Bitboards::BISHOP_MASKS[from] & allPieces);
        movesBoard &= maskFriendlyPieces;
        while (movesBoard) {
            Square to = Bitboards::popLSB(movesBoard);
            moves.push_back({ from, to });
        }
    }
}

void Chessboard::generateRookMoves(std::vector<Move>& moves, bool isWTurn) {
    Bitboard rooks = isWTurn ? whiteRooks : blackRooks;
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(isWTurn);
    Bitboard allPieces = getAllPieces();
    while (rooks) {
        Square from = Bitboards::popLSB(rooks);
        Bitboard movesBoard = Bitboards::getRookMoveTable(from, Bitboards::ROOK_MASKS[from] & allPieces);
        movesBoard &= maskFriendlyPieces;
        while (movesBoard) {
            Square to = Bitboards::popLSB(movesBoard);
            moves.push_back({ from, to });
        }
    }
}

void Chessboard::generateQueenMoves(std::vector<Move>& moves, bool isWTurn) {
    Bitboard queens = isWTurn ? whiteQueens : blackQueens;
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(isWTurn);
    Bitboard allPieces = getAllPieces();
    while (queens) {
        Square from = Bitboards::popLSB(queens);
        Bitboard movesBoard = Bitboards::getRookMoveTable(from, Bitboards::ROOK_MASKS[from] & allPieces);
        movesBoard |= Bitboards::getBishopMoveTable(from, Bitboards::BISHOP_MASKS[from] & allPieces);;
        movesBoard &= maskFriendlyPieces;
        while (movesBoard) {
            Square to = Bitboards::popLSB(movesBoard);
            moves.push_back({ from, to });
        }
    }
}

void Chessboard::display() {
    for (int r = RANK_8; r >= RANK_1; r--) {
        for (int f = FILE_A; f <= FILE_H; f++) {
            if (Bitboards::contains(whitePawns, Squares::fromRankFile(r, f))) {
                printf("P");
            } 
            else if (Bitboards::contains(blackPawns, Squares::fromRankFile(r, f))) {
                printf("p");
            } 
            else if (Bitboards::contains(whiteKnights, Squares::fromRankFile(r, f))) {
                printf("N");
            }
            else if (Bitboards::contains(blackKnights, Squares::fromRankFile(r, f))) {
                printf("n");
            }
            else if (Bitboards::contains(whiteBishops, Squares::fromRankFile(r, f))) {
                printf("B");
            }
            else if (Bitboards::contains(blackBishops, Squares::fromRankFile(r, f))) {
                printf("b");
            }
            else if (Bitboards::contains(whiteRooks, Squares::fromRankFile(r, f))) {
                printf("R");
            }
            else if (Bitboards::contains(blackRooks, Squares::fromRankFile(r, f))) {
                printf("r");
            }
            else if (Bitboards::contains(whiteQueens, Squares::fromRankFile(r, f))) {
                printf("Q");
            }
            else if (Bitboards::contains(blackQueens, Squares::fromRankFile(r, f))) {
                printf("q");
            }
            else if (Bitboards::contains(whiteKings, Squares::fromRankFile(r, f))) {
                printf("K");
            }
            else if (Bitboards::contains(blackKings, Squares::fromRankFile(r, f))) {
                printf("k");
            }
            else {
                printf(" ");
            }
        }
        printf("\n");
    }
}