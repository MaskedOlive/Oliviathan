#pragma once

#include "board.h"
#include <cstdint>

// The Evaluate class provides static methods to assess the quality of a chess position.
// This is a basic yet extensible evaluation framework suitable for an engine aiming for 1500 Elo.
// It includes material evaluation, simple piece-square tables, and basic positional considerations.
// All code is commented in UK English for clarity and future development.

class Evaluate {
public:
    // Evaluates the board position and returns a score (positive for White, negative for Black).
    // Units are centipawns (1 pawn = 100).
    static int score(const Board& board);

private:
    // Material values for each piece type; can be tuned for engine strength.
    static constexpr int PAWN_VALUE   = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE   = 500;
    static constexpr int QUEEN_VALUE  = 900;
    static constexpr int KING_VALUE   = 0; // King is invaluable (no material value).

    // Piece-square tables for basic positional evaluation.
    // These tables give small bonuses for piece placement.
    static const int pawnTable[64];
    static const int knightTable[64];
    static const int bishopTable[64];
    static const int rookTable[64];
    static const int queenTable[64];
    static const int kingTable[64];

    // Helper: Returns material score for a given piece type.
    static int getMaterialValue(Board::Piece piece);

    // Helper: Returns piece-square table value for a given piece, square, and colour.
    static int getPieceSquareValue(Board::Piece piece, int square, Board::Colour colour);

    // Helper: Evaluates castling rights (bonus for retaining ability to castle).
    static int evaluateCastling(const Board& board);

    // Helper: Evaluates pawn structure (basic doubled pawn penalty).
    static int evaluatePawnStructure(const Board& board);

    // Helper: Evaluates mobility (number of legal moves).
    static int evaluateMobility(const Board& board);

    // Helper: Evaluates checkmate and stalemate (large score for winning/losing/drawing positions).
    static int evaluateGameStatus(const Board& board);
};
