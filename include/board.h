#pragma once

#include <array>
#include <string>
#include <vector>
#include <cstdint>

// The Board class encapsulates the state and behaviour of a chessboard.
// It provides methods for initialisation, move application, display, 
// and querying the status of the game.

class Board {
public:
    // Constants representing board dimensions
    static constexpr int BOARD_SIZE = 8;
    static constexpr int NUM_SQUARES = 64;

    // Piece and colour enumeration for clarity
    enum Piece : uint8_t {
        EMPTY = 0,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };

    enum Colour : uint8_t {
        WHITE = 0,
        BLACK = 1,
        NO_COLOUR = 2
    };

    // Struct representing a piece on the board
    struct Square {
        Piece piece;
        Colour colour;

        Square() : piece(EMPTY), colour(NO_COLOUR) {}
        Square(Piece p, Colour c) : piece(p), colour(c) {}
    };

    // Constructor
    Board();

    // Resets the board to the standard chess starting position
    void reset();

    // Prints a textual representation of the board to the terminal
    void display() const;

    // Applies a move to the board, given in algebraic notation (e.g., "e2e4")
    // Returns true if the move was successfully applied
    bool makeMove(const std::string& move);

    // Determines if the game has ended (checkmate, stalemate, etc.)
    bool isGameOver() const;

    // Returns which colour is to move next
    Colour getSideToMove() const;

    // Returns a FEN (Forsyth-Edwards Notation) string representing the board position
    std::string getFEN() const;

    // Returns the piece and colour on a given square (0-63)
    Square getSquare(int index) const;

    // Utility: converts file/rank to square index
    static int toIndex(int file, int rank);

private:
    // The board is represented as an array of 64 squares
    std::array<Square, NUM_SQUARES> squares;

    // Tracks whose turn it is
    Colour sideToMove;

    // Castling rights: [white kingside, white queenside, black kingside, black queenside]
    std::array<bool, 4> castlingRights;

    // En passant target square (-1 if none)
    int enPassantSquare;

    // Counts halfmoves since last capture or pawn move (for 50-move rule)
    int halfmoveClock;

    // Counts the number of full moves
    int fullmoveNumber;

    // Helper function: initialise the board with pieces in their starting positions
    void initialisePosition();

    // Helper function: parses algebraic move notation to indices
    bool parseMove(const std::string& move, int& from, int& to) const;
};
