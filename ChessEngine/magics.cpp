
#include <random>
#include <intrin.h>

#include "Bitboard.h"

namespace Bitboards {

    Bitboard getRookMoveTable(Square sq, Bitboard blockers) {
        Magic m = ROOK_MAGICS[sq];
        return ROOK_MOVES[sq][(m.magic * blockers) >> m.shift];
    }

    Bitboard getBishopMoveTable(Square sq, Bitboard blockers) {
        Magic m = BISHOP_MAGICS[sq];
        return BISHOP_MOVES[sq][(m.magic * blockers) >> m.shift];
    }

    /***
     * Generate a bitboard for a piece that slides in a specific direction until being blocked.
     * Imagine casting a ray in a specific direction.
     * Starting square not included in returned bitboard.
     */
    Bitboard travelDirectionUntilBlocked(Square start, Bitboard blockers, int rankDir, int fileDir) {
        Bitboard moves = 0;
        int rank = Squares::getRank(start) + rankDir;
        int file = Squares::getFile(start) + fileDir;

        while (rank >= RANK_1 && rank <= RANK_8 && file >= FILE_A && file <= FILE_H) {
            Bitboard move = oneAt(Squares::fromRankFile(rank, file));
            moves |= move;
            if (blockers & move) { // hit a blocker
                break;
            }
            rank += rankDir;
            file += fileDir;
        }

        return moves;
    }

    /*** 
     * Manually calculates all rook moves given a set of blockers. This should only be called when initializing the ROOK_MOVES table.
     */
    Bitboard calcRookMoves(Square sq, Bitboard blockers) {
        Bitboard moves = 0;
        moves |= travelDirectionUntilBlocked(sq, blockers,  1,  0); // travel north
        moves |= travelDirectionUntilBlocked(sq, blockers, -1,  0); // travel south
        moves |= travelDirectionUntilBlocked(sq, blockers,  0,  1); // travel east
        moves |= travelDirectionUntilBlocked(sq, blockers,  0, -1); // travel west
        return moves;
    }

    /***
     * Manually calculates all bishop moves given a set of blockers. This should only be called when initializing the BISHOP_MOVES table.
     */
    Bitboard calcBishopMoves(Square sq, Bitboard blockers) {
        Bitboard moves = 0;
        moves |= travelDirectionUntilBlocked(sq, blockers,  1,  1); // travel ne
        moves |= travelDirectionUntilBlocked(sq, blockers, -1,  1); // travel se
        moves |= travelDirectionUntilBlocked(sq, blockers, -1, -1); // travel sw
        moves |= travelDirectionUntilBlocked(sq, blockers,  1, -1); // travel nw
        return moves;
    }

    /***
     * Iterate through all possible blocker bitboards and builds magic table for a specified piece type.
     * Returns whether or not the candidate is a valid magic.
     */
    bool tryMakeMagicTable(Square sq, Magic candidate, std::vector<Bitboard>& calculatedMovesForAllBlockers, bool isRook) {
        /*
        * To iterate through all permutations of n bits, we can simply
        * iterate from 0 - 2^n
        * However, we only want to iterate through the permuations of the bits set in the mask,
        * skipping over certain bits.
        * Ex: for a mask of 0101, our permutations are:
        *    0000
        *    0001
        *    0100
        *    0101
        *
        * Carry-Rippler technique to reach the next bitboard:
        * Let b be an existing blocker bitboard that we would like to increment, and m be our mask
        * 1. Set all bits that aren't used in our mask to 1 in b                  b | ~m
        * 2. Increment b, causing a ripple effect with a potential carry         (b | ~m) + 1
        * 3. Clear the unused bits by &ing with original mask                   ((b | ~m) + 1) & m
        *
        * Simplifying:
        * Since b is a subset of m (all bits that aren't set in m are also not set in b), we can + instead of |
        * since there is no risk of a carry
        *     ((b + ~m) + 1) & m
        * Property of one's complement: ~m = -m - 1
        *     ((b + (-m - 1)) + 1) & m = (b - m) & m
        */
        Bitboard mask = isRook ? ROOK_MASKS[sq] : BISHOP_MASKS[sq];

        Bitboard* outMoveTable = isRook ? ROOK_MOVES[sq] : BISHOP_MOVES[sq];
        bool usedIndices[MAX_ROOK_ATTACK_SETS] = { }; // since MAX_ROOK_ATTACK_SETS > MAX_BISHOP_ATTACK_SETS, this will ensure we always have enough space

        Bitboard blockers = 0;
        int counter = 0;
        do {
            int idx = (candidate.magic * blockers) >> candidate.shift;
            Bitboard moves = calculatedMovesForAllBlockers[counter];
            if (usedIndices[idx] && outMoveTable[idx] != moves) {
                /*
                * if some other blocker set hashed to this index but had a different move set, then
                * this magic is invalid (hash collision)
                */
                return false;
            }
            outMoveTable[idx] = moves;
            usedIndices[idx] = true;

            blockers = (blockers - mask) & mask;
            counter++;
        } while (blockers != 0); // blockers will reset back to 0 after iterating up to mask: (b - m) & m is 0 when b = m

        // this magic works! add it to our magic array:
        if (isRook) {
            ROOK_MAGICS[sq] = candidate;
        }
        else {
            BISHOP_MAGICS[sq] = candidate;
        }

        return true;
    }

    /***
     * Returns a vector of bitboards representing the possible moves of a given piece (rook/bishop) at a starting square
     * for all possible combinations of blockers.
     */
    std::vector<Bitboard> generateMovesForAllBlockers(Square sq, bool isRook) {
        Bitboard blockers = 0;
        Bitboard mask = isRook ? ROOK_MASKS[sq] : BISHOP_MASKS[sq];
        std::vector<Bitboard> movesForAllBlockers(isRook ? MAX_ROOK_ATTACK_SETS : MAX_BISHOP_ATTACK_SETS);
        do {
            movesForAllBlockers.push_back(isRook ? calcRookMoves(sq, blockers) : calcBishopMoves(sq, blockers));
            blockers = (blockers - mask) & mask;
        } while (blockers != 0);
        
        return movesForAllBlockers;
    }

    void generateMagics() {
        std::mt19937_64 rng;
        std::uniform_int_distribution<Bitboard> dist;
        for (int rank = RANK_1; rank <= RANK_8; rank++) {
            for (int file = FILE_A; file <= FILE_H; file++) {
                Magic candidate;
                bool foundRookMagic = false;
                bool foundBishopMagic = false;
                Square sq = Squares::fromRankFile(rank, file);
                std::vector<Bitboard> rookMovesForAllBlockers = generateMovesForAllBlockers(sq, true);
                std::vector<Bitboard> bishopMovesForAllBlockers = generateMovesForAllBlockers(sq, false);
                while (!foundRookMagic || !foundBishopMagic) {
                    // generate a random number with low # set bits
                    candidate.magic = dist(rng) & dist(rng) & dist(rng);

                    if (!foundRookMagic) {
                        // count number of bits in rook mask
                        int numBitsInMask = __popcnt64(ROOK_MASKS[sq]);
                        candidate.shift = NUM_SQUARES - numBitsInMask;
                        foundRookMagic = tryMakeMagicTable(sq, candidate, rookMovesForAllBlockers, true);
                    }

                    if (!foundBishopMagic) {
                        // count number of bits in bishop mask
                        int numBitsInMask = __popcnt64(BISHOP_MASKS[sq]);
                        candidate.shift = NUM_SQUARES - numBitsInMask;
                        foundBishopMagic = tryMakeMagicTable(sq, candidate, bishopMovesForAllBlockers, false);
                    }
                }
            }
        }
    }
}