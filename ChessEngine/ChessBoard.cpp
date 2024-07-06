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
            square = (Square)(square - 8); // go to the next rank
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

    idx++; // skip space
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
    int spaceIdx = halfMoveClockAndNumMoves.find(' ');
    halfMoveClock = std::stoi(halfMoveClockAndNumMoves.substr(0, spaceIdx));
    fullMoveNumber = std::stoi(halfMoveClockAndNumMoves.substr(spaceIdx));
}


Chessboard::generatePawnMoves() {

}