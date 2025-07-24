#pragma once

#include <array>
#include <string>
#include <vector>
#include <cstdint>

// The Board class models the chessboard and manages its state and operations.
// This implementation is designed for extensibility and clarity, suitable for a chess engine aiming at 1500 ELO.
// Detailed comments (in UK English) are provided to explain each feature, including support for castling and en passant.

class Board {
public:
    // Constants for board dimensions
    static constexpr int BOARD_SIZE = 8;       // 8x8 board
    static constexpr int NUM_SQUARES = 64;     // 64 squares

    // Enumeration for piece types
    enum Piece : uint8_t {
        EMPTY = 0,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };

    // Enumeration for piece colours
    enum Colour : uint8_t {
        WHITE = 0,
        BLACK = 1,
        NO_COLOUR = 2
    };

    // Structure representing a square on the board (piece and colour)
    struct Square {
        Piece piece;
        Colour colour;

        Square() : piece(EMPTY), colour(NO_COLOUR) {}
        Square(Piece p, Colour c) : piece(p), colour(c) {}
    };

    // Structure representing a basic move (can be extended for more info)
    struct Move {
        int from;             // Source square index (0-63)
        int to;               // Destination square index (0-63)
        Piece promotion;      // Promotion piece (if any)
        bool isCastle;        // True if this move is a castling move
        bool isEnPassant;     // True if this move is an en passant capture

        Move(int f, int t, Piece promo = EMPTY, bool castle = false, bool ep = false)
            : from(f), to(t), promotion(promo), isCastle(castle), isEnPassant(ep) {}
    };

    // Constructor: initialises the board
    Board();

    // Resets the board to the standard chess starting position
    void reset();

    // Prints the board in a human-readable format
    void display() const;

    // Applies a move (as a Move struct)
    // Returns true if the move is successfully applied
    bool makeMove(const Move& move);

    // Applies a move given in algebraic notation ("e2e4")
    bool makeMove(const std::string& moveStr);

    // Checks if the game is over (checkmate, stalemate, etc.)
    bool isGameOver() const;

    // Returns the colour whose turn it is to move
    Colour getSideToMove() const;

    // Returns a FEN string representing the board position
    std::string getFEN() const;

    // Returns the piece and colour at a given square index
    Square getSquare(int index) const;

    // Utility: converts file/rank to square index
    static int toIndex(int file, int rank);

    // Utility: converts square index to (file, rank)
    static void indexToCoords(int index, int& file, int& rank);

    // Get castling rights [white kingside, white queenside, black kingside, black queenside]
    std::array<bool, 4> getCastlingRights() const;

    // Get en passant target square (-1 if none)
    int getEnPassantSquare() const;

private:
    // The board is represented as an array of 64 squares
    std::array<Square, NUM_SQUARES> squares;

    // Tracks whose turn it is
    Colour sideToMove;

    // Castling rights: [white kingside, white queenside, black kingside, black queenside]
    std::array<bool, 4> castlingRights;

    // En passant target square index (-1 if none)
    int enPassantSquare;

    // Halfmove clock since last capture/pawn move (for 50-move rule)
    int halfmoveClock;

    // Fullmove number (increments after Black's move)
    int fullmoveNumber;

    // Helper: initialises pieces in their starting positions
    void initialisePosition();

    // Helper: parses algebraic move notation to indices and flags
    bool parseMove(const std::string& moveStr, Move& move) const;

    // Helper: clears en passant square after move unless just set
    void clearEnPassant();

    // Helper: updates castling rights if king or rook moves
    void updateCastlingRights(const Move& move);

    // Helper: checks if castling move is legal (conditions met)
    bool isLegalCastle(const Move& move) const;

    // Helper: checks if en passant move is legal (conditions met)
    bool isLegalEnPassant(const Move& move) const;

    // Helper: checks if a square is attacked by the specified colour
    bool isSquareAttacked(int square, Colour attacker) const;
};
