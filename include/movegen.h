#pragma once

#include "board.h"
#include <vector>
#include <string>

// The MoveGen class is responsible for generating all legal or pseudo-legal moves
// for a given board position. It is designed to be extensible so that new move
// generation techniques and optimisations can be added as the engine develops.
//
// This class generates moves in a simple format (from square, to square, promotion piece, etc.)
// and provides interfaces for generating moves for the current player, checking legality,
// and listing special moves (e.g., castling, en passant).

class MoveGen {
public:
    // Structure representing a chess move
    struct Move {
        int from;           // Source square index (0-63)
        int to;             // Destination square index (0-63)
        Board::Piece promotion; // Promotion piece type (if applicable, EMPTY otherwise)
        bool isCastle;      // True if this move is a castling move
        bool isEnPassant;   // True if this move is an en passant capture

        // Default constructor for a basic move (no promotion or special flags)
        Move(int f, int t, Board::Piece promo = Board::EMPTY, bool castle = false, bool ep = false)
            : from(f), to(t), promotion(promo), isCastle(castle), isEnPassant(ep) {}
    };

    // Generates all pseudo-legal moves for the given board position (ignores checks)
    static std::vector<Move> generatePseudoLegalMoves(const Board& board);

    // Generates all legal moves for the given board position (removes moves that leave king in check)
    static std::vector<Move> generateLegalMoves(const Board& board);

    // Utility function to convert a Move into algebraic notation ("e2e4", "e7e8q", etc.)
    static std::string moveToString(const Move& move);

    // Checks if a given move is legal in the current position
    static bool isLegalMove(const Board& board, const Move& move);

    // Utility: Returns true if a given square index is on the board
    static bool isOnBoard(int index);

    // Utility: Returns true if the given file and rank are valid
    static bool isValidSquare(int file, int rank);

    // Utility: Returns an array of squares attacked by the given colour (for check detection etc.)
    static std::vector<int> squaresAttacked(const Board& board, Board::Colour attacker);

private:
    // Helper functions for generating moves for each piece type
    static void addPawnMoves(const Board& board, int from, std::vector<Move>& moves);
    static void addKnightMoves(const Board& board, int from, std::vector<Move>& moves);
    static void addBishopMoves(const Board& board, int from, std::vector<Move>& moves);
    static void addRookMoves(const Board& board, int from, std::vector<Move>& moves);
    static void addQueenMoves(const Board& board, int from, std::vector<Move>& moves);
    static void addKingMoves(const Board& board, int from, std::vector<Move>& moves);

    // Helper to check if a given square is attacked (used for king safety & castling)
    static bool isSquareAttacked(const Board& board, int square, Board::Colour attacker);

    // Helper to handle castling moves
    static void addCastlingMoves(const Board& board, std::vector<Move>& moves);

    // Helper to handle en passant captures
    static void addEnPassantMoves(const Board& board, std::vector<Move>& moves);
};
