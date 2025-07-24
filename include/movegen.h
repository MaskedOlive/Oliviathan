#pragma once

#include "board.h"
#include <vector>
#include <string>

// The MoveGen class is responsible for generating all moves (pseudo-legal and legal)
// for the current position on the board. This implementation is designed for clarity,
// extensibility, and includes support for all key chess rules (including castling and en passant).
// Detailed UK English comments are included throughout to explain the logic and structure.

class MoveGen {
public:
    // Generates all pseudo-legal moves for the current board position.
    // These moves may include some that leave the king in check.
    static std::vector<Board::Move> generatePseudoLegalMoves(const Board& board);

    // Generates only legal moves for the current board position (moves that do not leave the king in check).
    static std::vector<Board::Move> generateLegalMoves(const Board& board);

    // Utility function to convert a Move structure to algebraic notation ("e2e4", "e7e8q", etc.).
    static std::string moveToString(const Board::Move& move);

    // Checks if a given move is legal in the current position.
    static bool isLegalMove(const Board& board, const Board::Move& move);

private:
    // Helper functions for each piece type, making it easy to extend or modify move generation.
    static void addPawnMoves(const Board& board, int from, std::vector<Board::Move>& moves);
    static void addKnightMoves(const Board& board, int from, std::vector<Board::Move>& moves);
    static void addBishopMoves(const Board& board, int from, std::vector<Board::Move>& moves);
    static void addRookMoves(const Board& board, int from, std::vector<Board::Move>& moves);
    static void addQueenMoves(const Board& board, int from, std::vector<Board::Move>& moves);
    static void addKingMoves(const Board& board, int from, std::vector<Board::Move>& moves);

    // Helper to add castling moves for the current side if legal.
    static void addCastlingMoves(const Board& board, std::vector<Board::Move>& moves);

    // Helper to add en passant capture moves if available.
    static void addEnPassantMoves(const Board& board, std::vector<Board::Move>& moves);

    // Utility to check if a given square is attacked by the opponent (used for legal move filtering and castling).
    static bool isSquareAttacked(const Board& board, int square, Board::Colour attacker);

    // Utility to find the king's square for a given colour.
    static int findKingSquare(const Board& board, Board::Colour colour);
};
