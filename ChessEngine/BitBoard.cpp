#include "Bitboard.h"

namespace Bitboards {

    Bitboard oneAt(Square sq) {
        return ((Bitboard)1) << sq;
    }

    bool contains(Bitboard board, Square sq) {
        return board && oneAt(sq);
    }

}
