#include "perft.h"
#include <iostream>

// Runs a perft test to the specified depth and returns the node count.
uint64_t Perft::run(const Board& board, int depth) {
    uint64_t nodes = 0;
    Board boardCopy = board;
    perftRecursive(boardCopy, depth, nodes);
    return nodes;
}

// Helper function for recursive perft calculation.
void Perft::perftRecursive(Board& board, int depth, uint64_t& nodes) {
    // Base case: depth 0 (leaf node)
    if (depth == 0) {
        nodes++;
        return;
    }

    // Generate all legal moves for the current position.
    auto moves = MoveGen::generateLegalMoves(board);

    // For each move, apply it, recurse, then undo (using board copy).
    for (const auto& move : moves) {
        Board boardCopy = board; // Copy board so each branch is independent.
        if (!boardCopy.makeMove(move)) continue; // Skip illegal moves (shouldn't happen with legal moves).
        perftRecursive(boardCopy, depth - 1, nodes);
    }
}

// Runs a perft test and provides detailed statistics about move types.
Perft::Results Perft::runDetailed(const Board& board, int depth) {
    Results results;
    Board boardCopy = board;
    perftRecursiveDetailed(boardCopy, depth, results);
    return results;
}

// Helper for recursive detailed perft.
void Perft::perftRecursiveDetailed(Board& board, int depth, Results& results) {
    if (depth == 0) {
        results.nodes++;
        return;
    }

    auto moves = MoveGen::generateLegalMoves(board);

    for (const auto& move : moves) {
        Board boardCopy = board;

        // Save move type stats at root level (depth 1)
        if (depth == 1) {
            // Captures: if destination square contains opponent's piece or en passant
            auto dest = boardCopy.getSquare(move.to);
            if ((dest.piece != Board::EMPTY && dest.colour != boardCopy.getSideToMove()) || move.isEnPassant)
                results.captures++;

            // Promotions
            if (move.promotion != Board::EMPTY)
                results.promotions++;

            // Castling
            if (move.isCastle)
                results.castles++;

            // En passant
            if (move.isEnPassant)
                results.enPassants++;

            // Checks
            Board tempBoard = boardCopy;
            if (tempBoard.makeMove(move) && isCheck(tempBoard, move))
                results.checks++;
        }

        if (!boardCopy.makeMove(move)) continue;
        perftRecursiveDetailed(boardCopy, depth - 1, results);
    }
}

// Utility: Checks if a move delivers check.
// For 1500 Elo, this is a simple implementation: after the move, is opponent's king attacked?
bool Perft::isCheck(const Board& board, const Board::Move& move) {
    Board::Colour opponent = (board.getSideToMove() == Board::WHITE) ? Board::BLACK : Board::WHITE;
    int kingSq = MoveGen::findKingSquare(board, opponent);
    if (kingSq == -1) return false; // Shouldn't happen in normal chess
    return MoveGen::isSquareAttacked(board, kingSq, board.getSideToMove());
}
