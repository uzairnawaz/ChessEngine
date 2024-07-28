
#include <sstream>

#include "Chessboard.h"

namespace Moves {
    std::string toString(Move& m) {
        std::string out = Squares::toAlgebraic(m.from) + Squares::toAlgebraic(m.to);
        if (m.promotion != Piece::PIECE_NONE) {
            char pieceNames[] = { 'p', 'n', 'b', 'r', 'q' };
            out += pieceNames[m.promotion];
        }
        return out;
    }
}

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
    Square square = A8;
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
        enPassantTarget = Square::SQUARE_NONE;
        idx += 2; // skip past '- '
    }

    // Load half move clock and full move number
    std::string halfMoveClockAndNumMoves = fen.substr(idx);
    size_t spaceIdx = halfMoveClockAndNumMoves.find(' ');
    halfMoveClock = std::stoi(halfMoveClockAndNumMoves.substr(0, spaceIdx));
    fullMoveNumber = std::stoi(halfMoveClockAndNumMoves.substr(spaceIdx));
}

Player Chessboard::getTurn() {
    return currentTurn;
}

int Chessboard::countPieces(Player player, Piece piece) {
    return __popcnt64(pieces[player + piece]);
}

Bitboard Chessboard::getAllPiecesByColor(Player color) {
    Bitboard out = 0;
    for (int p = Piece::PAWN; p <= Piece::KING; p++) {
        out |= pieces[color + p];
    }
    return out;
}

std::vector<Move> Chessboard::generateAllLegalMoves() {
    std::vector<Move> psuedolegalMoves = generateAllPseudolegalMoves();
    std::vector<Move> legalMoves;
    for (Move& m : psuedolegalMoves) {
        MoveUndoInfo moveInfo = makeMove(m);
        if (!isChecked(Players::getEnemy(currentTurn))) {
            // if our move didn't result in the next player being able to attack the king, the move is legal
            legalMoves.push_back(m);
        }
        undoMove(moveInfo);
    }
    return legalMoves;
}

std::vector<Move> Chessboard::generateAllPseudolegalMoves() {
    std::vector<Move> moves;

    generateKnightMoves(moves);
    generateKingMoves(moves);
    generateBishopMoves(moves);
    generateRookMoves(moves);
    generateQueenMoves(moves);

    /* After generating all moves, update isCapture property.
       This could've been done within each generate method,
       but would cause duplicate code. 
       
       Note: it is very easy to tell if a pawn move is a capture, 
             so generatePawnMoves handles that for us. 
       */
    Bitboard enemyPieces = getAllPiecesByColor(Players::getEnemy(currentTurn));
    for (Move& m : moves) {
        Bitboard target = Bitboards::oneAt(m.to);
        if (enemyPieces & target) {
            m.isCapture = true;
        }
    }

    generatePawnMoves(moves);

    return moves;
}

bool Chessboard::isAttacking(Player player, Square sq) {
    Bitboard allPieces = getAllPieces();
    // check pawn attacks
    Bitboard* pawnAttacks = player == Player::WHITE ? Bitboards::PAWN_ATTACKS_BLACK : Bitboards::PAWN_ATTACKS_WHITE;
    if (pawnAttacks[sq] & pieces[player + Piece::PAWN]) {
        return true;
    }
    if (Bitboards::KNIGHT_MOVES[sq] & pieces[player + Piece::KNIGHT]) {
        return true;
    }
    if (Bitboards::getBishopMoveTable(sq, Bitboards::BISHOP_MASKS[sq] & allPieces) & 
        (pieces[player + Piece::BISHOP] | pieces[player + Piece::QUEEN])) {
        return true;
    }
    if (Bitboards::getRookMoveTable(sq, Bitboards::ROOK_MASKS[sq] & allPieces) &
        (pieces[player + Piece::ROOK] | pieces[player + Piece::QUEEN])) {
        return true;
    }
    if (Bitboards::KING_MOVES[sq] & pieces[player + Piece::KING]) {
        return true;
    }
    return false;
}

bool Chessboard::isChecked(Player p) {
    unsigned long kingLoc;
    _BitScanForward64(&kingLoc, pieces[p + Piece::KING]);
    return isAttacking(Players::getEnemy(p), (Square)kingLoc);
}

void Chessboard::generatePawnMoves(std::vector<Move>& moves) {
    Bitboard pawns = pieces[Piece::PAWN + currentTurn];
    Bitboard allPieces = getAllPieces();
    // imagine an enemy piece is at en passant target
    Bitboard enemyPieces = getAllPiecesByColor(Players::getEnemy(currentTurn)) | Bitboards::oneAt(enPassantTarget);
    Rank promoteRank = currentTurn == Player::WHITE ? RANK_8 : RANK_1;
    Rank startingRank = currentTurn == Player::WHITE ? RANK_2 : RANK_7;
    while (pawns) {
        Square from = Bitboards::popLSB(pawns);
        Bitboard movesBoard = currentTurn == Player::WHITE ? Bitboards::oneAt((Square)(from + 8)) : Bitboards::oneAt((Square)(from - 8));
        movesBoard = movesBoard & ~allPieces;
        Bitboard attacksBoard = currentTurn == Player::WHITE ? Bitboards::PAWN_ATTACKS_WHITE[from] : Bitboards::PAWN_ATTACKS_BLACK[from];
        attacksBoard = attacksBoard & enemyPieces;
        while (movesBoard) {
            Square to = Bitboards::popLSB(movesBoard);
            if (Squares::getRank(to) == promoteRank) {
                moves.push_back({ from, to, Piece::KNIGHT });
                moves.push_back({ from, to, Piece::BISHOP });
                moves.push_back({ from, to, Piece::ROOK });
                moves.push_back({ from, to, Piece::QUEEN });
            }
            else {
                moves.push_back({ from, to });
            }

            // double push
            Square doublePushSquare = (Square)(currentTurn == Player::WHITE ? to + 8 : to - 8);
            Bitboard doublePush = Bitboards::oneAt(doublePushSquare);
            if (Squares::getRank(from) == startingRank && (doublePush & allPieces) == 0) {
                moves.push_back({ from, doublePushSquare });
            }
        }
        while (attacksBoard) {
            Square to = Bitboards::popLSB(attacksBoard);
            if (Squares::getRank(to) == promoteRank) {
                moves.push_back({ from, to, Piece::KNIGHT, true /* isCapture */});
                moves.push_back({ from, to, Piece::BISHOP, true });
                moves.push_back({ from, to, Piece::ROOK, true });
                moves.push_back({ from, to, Piece::QUEEN, true });
            }
            else {
                moves.push_back({ from, to, Piece::PIECE_NONE, true});
            }
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
    Bitboard allPieces = getAllPieces();
    Square from = Bitboards::popLSB(king);
    Bitboard movesBoard = Bitboards::KING_MOVES[from] & maskFriendlyPieces;
    
   
    Bitboard kingSideMask = currentTurn == Player::WHITE ? Bitboards::WHITE_KINGSIDE : Bitboards::BLACK_KINGSIDE;
    Bitboard queenSideMask = currentTurn == Player::WHITE ? Bitboards::WHITE_QUEENSIDE : Bitboards::BLACK_QUEENSIDE;

    while (movesBoard) {
        Square to = Bitboards::popLSB(movesBoard);
        moves.push_back({ from, to });
    }

    if (!isChecked(currentTurn)) {
        if (currentTurn == Player::WHITE) {
            if (castleAbility.wKingside && (allPieces & Bitboards::WHITE_KINGSIDE) == 0 && 
                !isAttacking(Player::BLACK, Square::F1) && !isAttacking(Player::BLACK, Square::G1)) {
                moves.push_back({ E1, G1 });
            }
            if (castleAbility.wQueenside && (allPieces & Bitboards::WHITE_QUEENSIDE) == 0 &&
                !isAttacking(Player::BLACK, Square::D1) && !isAttacking(Player::BLACK, Square::C1)) {
                moves.push_back({ E1, C1 });
            }
        }
        else {
            if (castleAbility.bKingside && (allPieces & Bitboards::BLACK_KINGSIDE) == 0 &&
                !isAttacking(Player::WHITE, Square::F8) && !isAttacking(Player::WHITE, Square::G8)) {
                moves.push_back({ E8, G8 });
            }
            if (castleAbility.bQueenside && (allPieces & Bitboards::BLACK_QUEENSIDE) == 0 &&
                !isAttacking(Player::WHITE, Square::D8) && !isAttacking(Player::WHITE, Square::C8)) {
                moves.push_back({ E8, C8 });
            }
        }
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
    while (piece != Piece::PIECE_NONE && (pieces[player + piece] & bb) == 0) {
        piece = (Piece)(piece + 1);
    }
    return piece;
}

MoveUndoInfo Chessboard::makeMove(Move m) {
    Bitboard fromBB = Bitboards::oneAt(m.from);
    Bitboard toBB = Bitboards::oneAt(m.to);
    Piece fromPiece = getPieceTypeAtSquareGivenColor(m.from, currentTurn);
    CastleAbility oldCastleAbility = castleAbility;

    // check if there is an enemy piece at destination
    Piece toPiece = getPieceTypeAtSquareGivenColor(m.to, Players::getEnemy(currentTurn));

    // perform move
    pieces[currentTurn + fromPiece] &= ~fromBB; // remove piece from old location
    if (m.promotion == Piece::PIECE_NONE) {
        pieces[currentTurn + fromPiece] |= toBB;    // add piece to new location
    }
    else {
        pieces[currentTurn + m.promotion] |= toBB;  // promote pawn
    }

    bool isCapture = false;
    if (toPiece != Piece::PIECE_NONE) {
        isCapture = true;
        // if this move is a capture, remove enemy piece
        pieces[Players::getEnemy(currentTurn) + toPiece] &= ~toBB;

        // if rook captured, can't castle on that side
        if (toPiece == Piece::ROOK) {
            switch (m.to) {
            case Square::A1:
                castleAbility.wQueenside = false;
                break;
            case Square::H1:
                castleAbility.wKingside = false;
                break;
            case Square::A8:
                castleAbility.bQueenside = false;
                break;
            case Square::H8:
                castleAbility.bKingside = false;
            }
        }
    }
    else if (fromPiece == Piece::PAWN && m.to == enPassantTarget) {
        isCapture = true;
        // if this move is an en passant, remove enemy pawn
        Rank r = Squares::getRank(enPassantTarget);
        File f = Squares::getFile(enPassantTarget);
        // enemy pawn is either 1 rank above or 1 rank below en passant target based on player color
        Square enemyPawnToKill = Squares::fromRankFile(currentTurn == Player::WHITE ? r - 1 : r + 1, f);
        pieces[Players::getEnemy(currentTurn) + Piece::PAWN] &= ~Bitboards::oneAt(enemyPawnToKill);
    }

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

        if (Squares::getFile(m.from) == File::FILE_E) {
            // if castling kingside
            if (Squares::getFile(m.to) == File::FILE_G) {
                // move kingside rook. can assume it is at H file because we assume castling is a valid move
                Bitboard kingsideRook = Bitboards::oneAt(currentTurn == Player::WHITE ? Square::H1 : Square::H8);
                Bitboard newRookLoc = Bitboards::oneAt(currentTurn == Player::WHITE ? Square::F1 : Square::F8);
                pieces[currentTurn + Piece::ROOK] &= ~kingsideRook;
                pieces[currentTurn + Piece::ROOK] |= newRookLoc;
            }
            // if castling queenside
            if (Squares::getFile(m.to) == File::FILE_C) {
                // move queenside rook. can assume it is at A file because we assume castling is a valid move
                Bitboard queensideRook = Bitboards::oneAt(currentTurn == Player::WHITE ? Square::A1 : Square::A8);
                Bitboard newRookLoc = Bitboards::oneAt(currentTurn == Player::WHITE ? Square::D1 : Square::D8);
                pieces[currentTurn + Piece::ROOK] &= ~queensideRook;
                pieces[currentTurn + Piece::ROOK] |= newRookLoc;
            }
        }
    }
    else if (fromPiece == Piece::ROOK) {
        // if we moved a rook, we can no longer castle on that side
        switch (m.from) {
        case Square::A1:
            castleAbility.wQueenside = false;
            break;
        case Square::H1:
            castleAbility.wKingside = false;
            break;
        case Square::A8:
            castleAbility.bQueenside = false;
            break;
        case Square::H8:
            castleAbility.bKingside = false;
        }
    }

    Square oldEnPassantTarget = enPassantTarget;
    if (fromPiece == Piece::PAWN && (m.to - m.from == 16 || m.from - m.to == 16))
    {
        // if this move double pushed a pawn, it is now an en passant target
        enPassantTarget = (Square)(currentTurn == Player::WHITE ? m.from + 8 : m.from - 8);
    }
    else {
        enPassantTarget = Square::SQUARE_NONE;
    }

    if (currentTurn == Player::BLACK) {
        fullMoveNumber++;
    }

    int oldHalfMoveClock = halfMoveClock;
    if (isCapture || fromPiece == Piece::PAWN) {
        halfMoveClock = 0;
    }
    else {
        halfMoveClock++;
    }

    currentTurn = Players::getEnemy(currentTurn);
    

    return { m, toPiece, oldCastleAbility, oldEnPassantTarget, oldHalfMoveClock };
}

void Chessboard::undoMove(MoveUndoInfo m) {
    currentTurn = Players::getEnemy(currentTurn);

    Bitboard fromBB = Bitboards::oneAt(m.move.from);
    Bitboard toBB = Bitboards::oneAt(m.move.to);

    Piece p = getPieceTypeAtSquareGivenColor(m.move.to, currentTurn);

    pieces[currentTurn + p] &= ~toBB;  // remove piece from current location
    if (m.move.promotion == Piece::PIECE_NONE) {
        pieces[currentTurn + p] |= fromBB; // add piece to old location
    }
    else {
        pieces[currentTurn + Piece::PAWN] |= fromBB; // demote back to pawn
    }

    if (p == Piece::PAWN && m.move.to == m.enPassantTarget) {
        // bring back captured pawn from en passant
        Rank r = Squares::getRank(m.enPassantTarget);
        File f = Squares::getFile(m.enPassantTarget);
        Square enemyPawnLoc = Squares::fromRankFile(currentTurn == Player::WHITE ? r - 1 : r + 1, f);
        pieces[Players::getEnemy(currentTurn) + Piece::PAWN] |= Bitboards::oneAt(enemyPawnLoc);
    }
    else if (m.captured != Piece::PIECE_NONE) {
        // bring captured piece back on the board
        pieces[Players::getEnemy(currentTurn) + m.captured] |= toBB;
    }

    if (p == Piece::KING) {
        if (Squares::getFile(m.move.from) == File::FILE_E) {
            if (Squares::getFile(m.move.to) == File::FILE_G) {
                // undo kingside castle
                Bitboard originalLoc = Bitboards::oneAt(currentTurn == Player::WHITE ? Square::H1 : Square::H8);
                Bitboard curLoc = Bitboards::oneAt(currentTurn == Player::WHITE ? Square::F1 : Square::F8);
                pieces[currentTurn + Piece::ROOK] &= ~curLoc;
                pieces[currentTurn + Piece::ROOK] |= originalLoc;
            }
            if (Squares::getFile(m.move.to) == File::FILE_C) {
                // undo queenside castle
                Bitboard originalLoc = Bitboards::oneAt(currentTurn == Player::WHITE ? Square::A1 : Square::A8);
                Bitboard curLoc = Bitboards::oneAt(currentTurn == Player::WHITE ? Square::D1 : Square::D8);
                pieces[currentTurn + Piece::ROOK] &= ~curLoc;
                pieces[currentTurn + Piece::ROOK] |= originalLoc;
            }
        }
    }

    if (currentTurn == Player::BLACK) {
        fullMoveNumber--;
    }

    castleAbility = m.castleAbility;
    enPassantTarget = m.enPassantTarget;
    halfMoveClock = m.halfMoveClock;
}

unsigned long Chessboard::perft(int depth) {
    unsigned long numMoves = 0;
    std::vector<Move> moves = generateAllLegalMoves();
    if (depth == 0) {
        return 1;
    }
    if (depth == 1) {
        return moves.size();
    }
    for (Move& m : moves) {
        MoveUndoInfo moveInfo = makeMove(m);
        numMoves += perft(depth - 1);
        undoMove(moveInfo);
    }

    return numMoves;
}

unsigned long Chessboard::psuedolegalPerft(int depth) {
    unsigned long numMoves = 0;
    std::vector<Move> moves = generateAllPseudolegalMoves();
    if (depth == 0) {
        return 1;
    }
    for (Move& m : moves) {
        MoveUndoInfo moveInfo = makeMove(m);
        if (!isChecked(Players::getEnemy(currentTurn))) {
            numMoves += psuedolegalPerft(depth - 1);
        }
        undoMove(moveInfo);
    }

    return numMoves;
}

unsigned long Chessboard::verbosePerft(int depth) {
    unsigned long numMoves = 0;
    std::vector<Move> moves = generateAllLegalMoves();
    if (depth == 0) {
        return 1;
    }
    for (Move& m : moves) {
        MoveUndoInfo moveInfo = makeMove(m);
        unsigned long newMoves = perft(depth - 1);
        numMoves += newMoves;
        printf("%s%s: %d\n", Squares::toAlgebraic(m.from).c_str(),
            Squares::toAlgebraic(m.to).c_str(), newMoves);
        undoMove(moveInfo);
    }

    return numMoves;
}

std::string Chessboard::toFEN() {
    std::stringstream out;
    for (int r = RANK_8; r >= RANK_1; r--) {
        int numEmpties = 0;
        bool isEmptySquare;
        for (int f = FILE_A; f <= FILE_H; f++) {
            isEmptySquare = false;
            char piece;
            if (Bitboards::contains(pieces[Player::WHITE + Piece::PAWN], Squares::fromRankFile(r, f))) {
                piece = 'P';
            } 
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::PAWN], Squares::fromRankFile(r, f))) {
                piece = 'p';
            } 
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::KNIGHT], Squares::fromRankFile(r, f))) {
                piece = 'N';
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::KNIGHT], Squares::fromRankFile(r, f))) {
                piece = 'n';
            }
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::BISHOP], Squares::fromRankFile(r, f))) {
                piece = 'B';
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::BISHOP], Squares::fromRankFile(r, f))) {
                piece = 'b';
            }
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::ROOK], Squares::fromRankFile(r, f))) {
                piece = 'R';
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::ROOK], Squares::fromRankFile(r, f))) {
                piece = 'r';
            }
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::QUEEN], Squares::fromRankFile(r, f))) {
                piece = 'Q';
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::QUEEN], Squares::fromRankFile(r, f))) {
                piece = 'q';
            }
            else if (Bitboards::contains(pieces[Player::WHITE + Piece::KING], Squares::fromRankFile(r, f))) {
                piece = 'K';
            }
            else if (Bitboards::contains(pieces[Player::BLACK + Piece::KING], Squares::fromRankFile(r, f))) {
                piece = 'k';
            }
            else {
                isEmptySquare = true;
                numEmpties++;
            }

            if (!isEmptySquare) {
                if (numEmpties != 0) {
                    out << '0' + numEmpties;
                    numEmpties = 0;
                }
                out << piece;
            }
        }
        
        out << "/";
    }

    out.seekp(-1, out.cur); // remove extra '/' at the end
    out << " "; 

    // current turn
    out << (currentTurn == Player::WHITE ? 'w' : 'b');
    out << " ";

    // castle rights
    out << (castleAbility.wKingside  ? "K" : "");
    out << (castleAbility.wQueenside ? "Q" : "");
    out << (castleAbility.bKingside  ? "k" : "");
    out << (castleAbility.bQueenside ? "q" : "");
    out << " ";

    // en passant
    out << (enPassantTarget == Square::SQUARE_NONE ? "-" : Squares::toAlgebraic(enPassantTarget));
    out << " ";

    // half move clock
    out << std::to_string(halfMoveClock);
    out << " ";

    // full move number
    out << std::to_string(fullMoveNumber);

    return out.str();
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