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
            pieces[Player::BLACK + Piece::PAWN] |= Bitboards::oneAt(square);
            break;
        case 'P':
            pieces[Player::WHITE + Piece::PAWN] |= Bitboards::oneAt(square);
            break;
        case 'n':
            pieces[Player::BLACK + Piece::KNIGHT] |= Bitboards::oneAt(square);
            break;
        case 'N':
            pieces[Player::WHITE + Piece::KNIGHT] |= Bitboards::oneAt(square);
            break;
        case 'b':
            pieces[Player::BLACK + Piece::BISHOP] |= Bitboards::oneAt(square);
            break;
        case 'B':
            pieces[Player::WHITE + Piece::BISHOP] |= Bitboards::oneAt(square);
            break;
        case 'r':
            pieces[Player::BLACK + Piece::ROOK] |= Bitboards::oneAt(square);
            break;
        case 'R':
            pieces[Player::WHITE + Piece::ROOK] |= Bitboards::oneAt(square);
            break;
        case 'q':
            pieces[Player::BLACK + Piece::QUEEN] |= Bitboards::oneAt(square);
            break;
        case 'Q':
            pieces[Player::WHITE + Piece::QUEEN] |= Bitboards::oneAt(square);
            break;
        case 'k':
            pieces[Player::BLACK + Piece::KING] |= Bitboards::oneAt(square);
            break;
        case 'K':
            pieces[Player::WHITE + Piece::KING] |= Bitboards::oneAt(square);
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
    currentTurn = fen.at(idx) == 'w' ? Player::WHITE : Player::BLACK;

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
        enPassantTarget = Squares::fromAlgebraic(fen.substr(idx, 2).c_str());
        idx += 3; // skip past coordinate and space
    }
    else {
        enPassantTarget = SQ_NONE;
        idx += 2; // skip past '- '
    }

    // Load half move clock and full move number
    std::string halfMoveClockAndNumMoves = fen.substr(idx);
    size_t spaceIdx = halfMoveClockAndNumMoves.find(' ');
    halfMoveClock = std::stoi(halfMoveClockAndNumMoves.substr(0, spaceIdx));
    fullMoveNumber = std::stoi(halfMoveClockAndNumMoves.substr(spaceIdx));
}

Bitboard Chessboard::getAllPiecesByColor(Player color) {
    Bitboard out = 0;
    for (int p = Piece::PAWN; p <= Piece::KING; p++) {
        out |= pieces[color + p];
    }
    return out;
}

std::vector<Move> Chessboard::generateAllLegalMoves() {
    std::vector<Move> moves = generateAllPseudolegalMoves();
    return moves;
}

std::vector<Move> Chessboard::generateAllPseudolegalMoves() {
    std::vector<Move> moves;
    generatePawnMoves(moves);
    generateKnightMoves(moves);
    generateKingMoves(moves);
    generateBishopMoves(moves);
    generateRookMoves(moves);
    generateQueenMoves(moves);
    return moves;
}

void Chessboard::generatePawnMoves(std::vector<Move>& moves) {
    Bitboard pawns = pieces[Piece::PAWN + currentTurn];
    Bitboard maskAllPieces = ~getAllPieces();
    // imagine an enemy piece is at en passant target
    Bitboard enemyPieces = getAllPiecesByColor(Players::getEnemy(currentTurn)) | Bitboards::oneAt(enPassantTarget);
    while (pawns) {
        Square from = Bitboards::popLSB(pawns);
        Bitboard movesBoard = currentTurn == Player::WHITE ? Bitboards::PAWN_MOVES_WHITE[from] : Bitboards::PAWN_MOVES_BLACK[from];
        movesBoard = movesBoard & maskAllPieces;
        Bitboard attacksBoard = currentTurn == Player::WHITE ? Bitboards::PAWN_ATTACKS_WHITE[from] : Bitboards::PAWN_ATTACKS_BLACK[from];
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

void Chessboard::generateKnightMoves(std::vector<Move>& moves) {
    Bitboard knights = pieces[Piece::KNIGHT + currentTurn];
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(currentTurn);
    while (knights) {
        Square from = Bitboards::popLSB(knights);
        Bitboard movesBoard = Bitboards::KNIGHT_MOVES[from] & maskFriendlyPieces;
        while (movesBoard) {
            Square to = Bitboards::popLSB(movesBoard);
            moves.push_back({ from, to });
        }
    }
}

void Chessboard::generateKingMoves(std::vector<Move>& moves) {
    Bitboard king = pieces[Piece::KING + currentTurn];
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(currentTurn);
    Square from = Bitboards::popLSB(king);
    Bitboard movesBoard = Bitboards::KING_MOVES[from] & maskFriendlyPieces;
    while (movesBoard) {
        Square to = Bitboards::popLSB(movesBoard);
        moves.push_back({ from, to });
    }
}

void Chessboard::generateBishopMoves(std::vector<Move>& moves) {
    Bitboard bishops = pieces[Piece::BISHOP + currentTurn];
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(currentTurn);
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

void Chessboard::generateRookMoves(std::vector<Move>& moves) {
    Bitboard rooks = pieces[Piece::ROOK + currentTurn];
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(currentTurn);
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

void Chessboard::generateQueenMoves(std::vector<Move>& moves) {
    Bitboard queens = pieces[Piece::QUEEN + currentTurn];
    Bitboard maskFriendlyPieces = ~getAllPiecesByColor(currentTurn);
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

Piece Chessboard::getPieceTypeAtSquareGivenColor(Square s, Player player) {
    Piece piece = Piece::PAWN;
    Bitboard bb = Bitboards::oneAt(s);
    while (piece != Piece::NONE && (pieces[player + piece] & bb) == 0) {
        piece = (Piece)(piece + 1);
    }
    return piece;
}

MoveUndoInfo Chessboard::makeMove(Move m) {
    Bitboard fromBB = Bitboards::oneAt(m.from);
    Bitboard toBB = Bitboards::oneAt(m.to);
    Piece fromPiece = getPieceTypeAtSquareGivenColor(m.from, currentTurn);

    // check if there is an enemy piece at destination
    Piece toPiece = getPieceTypeAtSquareGivenColor(m.to, Players::getEnemy(currentTurn));

    // perform move
    pieces[currentTurn + fromPiece] &= ~fromBB; // remove piece from old location
    pieces[currentTurn + fromPiece] |= toBB;    // add piece to new location
    if (toPiece != Piece::NONE) {
        // if this move is a capture, remove enemy piece
        pieces[Players::getEnemy(currentTurn) + toPiece] &= ~toBB;
    }
    else if (fromPiece == Piece::PAWN && m.to == enPassantTarget) {
        // if this move is an en passant, remove enemy pawn
        Rank r = Squares::getRank(enPassantTarget);
        File f = Squares::getFile(enPassantTarget);
        // enemy pawn is either 1 rank above or 1 rank below en passant target based on player color
        Square enemyPawnToKill = Squares::fromRankFile(currentTurn == Player::WHITE ? r - 1 : r + 1, f);
        pieces[Players::getEnemy(currentTurn) + Piece::PAWN] &= ~Bitboards::oneAt(enemyPawnToKill);
    }

    CastleAbility oldCastleAbility = castleAbility;
    if (fromPiece == Piece::KING) {
        // if we moved the king, we can no longer castle
        if (currentTurn == Player::WHITE) {
            castleAbility.wKingside = false;
            castleAbility.wQueenside = false;
        }
        else {
            castleAbility.bKingside = false;
            castleAbility.bQueenside = false;
        }
    }
    else if (fromPiece == Piece::ROOK) {
        // if we moved a rook, we can no longer castle on that side
        switch (m.from) {
        case Square::SQ_A1:
            castleAbility.wQueenside = false;
            break;
        case Square::SQ_H1:
            castleAbility.wKingside = false;
            break;
        case Square::SQ_A8:
            castleAbility.bQueenside = false;
            break;
        case Square::SQ_H8:
            castleAbility.bKingside = false;
        }
    }

    Square oldEnPassantTarget = enPassantTarget;
    if (fromPiece == Piece::PAWN && (m.to - m.from == 16 || m.from - m.to == 16))
    {
        // if this move double pushed a pawn, it is now an en pessant target
        // don't need to check direction based on player since we assume move is legal
        enPassantTarget = m.to;
    }
    currentTurn = Players::getEnemy(currentTurn);

    return { m, toPiece, oldCastleAbility, oldEnPassantTarget };
}

void Chessboard::undoMove(MoveUndoInfo m) {
    currentTurn = Players::getEnemy(currentTurn);

    Bitboard fromBB = Bitboards::oneAt(m.move.from);
    Bitboard toBB = Bitboards::oneAt(m.move.to);

    Piece p = getPieceTypeAtSquareGivenColor(m.move.to, currentTurn);

    pieces[currentTurn + p] &= ~toBB;  // remove piece from current location
    pieces[currentTurn + p] |= fromBB; // add piece to old location

    if (m.captured != Piece::NONE) {
        // bring captured piece back on the board
        pieces[Players::getEnemy(currentTurn) + m.captured] |= toBB;
    }

    castleAbility = m.castleAbility;
    enPassantTarget = m.enPassantTarget;
}

std::string Chessboard::toString() {
    std::string out = "";
    for (int r = RANK_8; r >= RANK_1; r--) {
        for (int f = FILE_A; f <= FILE_H; f++) {
            if (Bitboards::contains(pieces[Player::WHITE + Piece::PAWN], Squares::fromRankFile(r, f))) {
                out += "P";
            } 
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::PAWN], Squares::fromRankFile(r, f))) {
                out += "p";
            } 
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::KNIGHT], Squares::fromRankFile(r, f))) {
                out += "N";
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::KNIGHT], Squares::fromRankFile(r, f))) {
                out += "n";
            }
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::BISHOP], Squares::fromRankFile(r, f))) {
                out += "B";
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::BISHOP], Squares::fromRankFile(r, f))) {
                out += "b";
            }
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::ROOK], Squares::fromRankFile(r, f))) {
                out += "R";
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::ROOK], Squares::fromRankFile(r, f))) {
                out += "r";
            }
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::QUEEN], Squares::fromRankFile(r, f))) {
                out += "Q";
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::QUEEN], Squares::fromRankFile(r, f))) {
                out += "q";
            }
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::KING], Squares::fromRankFile(r, f))) {
                out += "K";
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::KING], Squares::fromRankFile(r, f))) {
                out += "k";
            }
            else {
                out += " ";
            }
        }
        out += "\n";
    }

    return out;
}