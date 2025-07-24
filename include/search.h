#pragma once

#include "board.h"
#include "movegen.h"
#include "evaluate.h"
#include <cstdint>
#include <vector>
#include <limits>

// The Search class implements the thinking logic of the chess engine.
// This module searches for the best move using a minimax algorithm with alpha-beta pruning.
// It uses the Evaluate class to score positions and MoveGen for legal moves.
// The code is designed to be modular and extensible, with thorough UK English comments
// to help you understand every step and make improvements for higher Elo strengths.

class Search {
public:
    // Searches for the best move from the current position.
    // Returns the best move found and sets its evaluation score.
    static Board::Move findBestMove(const Board& board, int depth, int& outScore);

    // Returns only the best move (convenience overload).
    static Board::Move findBestMove(const Board& board, int depth);

    // Minimax search with alpha-beta pruning.
    // Returns the evaluation score for the given position.
    static int minimax(Board& board, int depth, int alpha, int beta, bool maximisingPlayer);

private:
    // Helper: Orders moves to improve alpha-beta efficiency (simple MVV/LVA).
    static std::vector<Board::Move> orderMoves(const Board& board, const std::vector<Board::Move>& moves);

    // Helper: Checks for game over and returns mate/stalemate scores.
    static int checkGameOver(const Board& board, int ply);
};
