#pragma once

#include "board.h"
#include "movegen.h"
#include <cstdint>
#include <vector>

// The Perft class is used for move generation testing in chess engines.
// "Perft" stands for "performance test" and is a standard tool to verify the correctness of your move generator.
// It counts all possible legal move sequences from a position up to a given depth.
// This implementation is modular and easy to extend, with detailed UK English comments for understanding and future improvement.

class Perft {
public:
    // Runs a perft test from the given board position to the specified depth.
    // Returns the number of leaf nodes (unique positions) found.
    static uint64_t run(const Board& board, int depth);

    // Runs a perft test and gives a breakdown of move types (captures, promotions, castles, en passant, checks).
    // Useful for in-depth debugging and engine validation.
    struct Results {
        uint64_t nodes = 0;          // Total leaf nodes
        uint64_t captures = 0;       // Number of captures
        uint64_t promotions = 0;     // Number of pawn promotions
        uint64_t castles = 0;        // Number of castling moves
        uint64_t enPassants = 0;     // Number of en passant captures
        uint64_t checks = 0;         // Number of checks
    };

    static Results runDetailed(const Board& board, int depth);

private:
    // Helper function for recursive perft counting
    static void perftRecursive(Board& board, int depth, uint64_t& nodes);

    // Helper for detailed perft (counts move types)
    static void perftRecursiveDetailed(Board& board, int depth, Results& results);

    // Utility: Checks if a move delivers check
    static bool isCheck(const Board& board, const Board::Move& move);
};
