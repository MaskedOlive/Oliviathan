#include "board.h"
#include <iostream>
#include <sstream>
#include <cctype>

// Constructor: sets up an empty board and initial values
Board::Board()
    : squares(), sideToMove(WHITE), castlingRights{true, true, true, true},
      enPassantSquare(-1), halfmoveClock(0), fullmoveNumber(1)
{
    reset(); // Initialise to standard chess position
}

// Resets the board to the standard chess starting position
void Board::reset() {
    initialisePosition();
    sideToMove = WHITE;
    castlingRights = {true, true, true, true};
    enPassantSquare = -1;
    halfmoveClock = 0;
    fullmoveNumber = 1;
}

// Helper: Initialises the board with pieces in their starting positions
void Board::initialisePosition() {
    // Set all squares to empty
    for (auto& sq : squares) {
        sq = Square();
    }
    // Place pawns
    for (int f = 0; f < BOARD_SIZE; ++f) {
        squares[toIndex(f, 1)] = Square(PAWN, WHITE);
        squares[toIndex(f, 6)] = Square(PAWN, BLACK);
    }
    // Place major pieces for both colours
    std::array<Piece, 8> backRank = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    for (int f = 0; f < BOARD_SIZE; ++f) {
        squares[toIndex(f, 0)] = Square(backRank[f], WHITE);
        squares[toIndex(f, 7)] = Square(backRank[f], BLACK);
    }
}

// Prints the board to the console in a human-readable format
void Board::display() const {
    std::cout << "\n   a b c d e f g h\n";
    std::cout << "  -----------------\n";
    for (int r = BOARD_SIZE - 1; r >= 0; --r) {
        std::cout << r + 1 << "| ";
        for (int f = 0; f < BOARD_SIZE; ++f) {
            const Square& sq = squares[toIndex(f, r)];
            char symbol = '.';
            if (sq.piece != EMPTY) {
                switch (sq.piece) {
                    case PAWN:   symbol = 'P'; break;
                    case KNIGHT: symbol = 'N'; break;
                    case BISHOP: symbol = 'B'; break;
                    case ROOK:   symbol = 'R'; break;
                    case QUEEN:  symbol = 'Q'; break;
                    case KING:   symbol = 'K'; break;
                    default:     symbol = '?';
                }
                if (sq.colour == BLACK)
                    symbol = std::tolower(symbol); // Black pieces in lowercase
            }
            std::cout << symbol << ' ';
        }
        std::cout << "|" << r + 1 << '\n';
    }
    std::cout << "  -----------------\n";
    std::cout << "   a b c d e f g h\n";
    std::cout << "Side to move: " << (sideToMove == WHITE ? "White" : "Black") << "\n";
    // Castling rights display
    std::cout << "Castling rights: "
              << (castlingRights[0] ? "K" : "")
              << (castlingRights[1] ? "Q" : "")
              << (castlingRights[2] ? "k" : "")
              << (castlingRights[3] ? "q" : "") << "\n";
    // En passant display
    if (enPassantSquare != -1) {
        int f = enPassantSquare % BOARD_SIZE;
        int r = enPassantSquare / BOARD_SIZE;
        std::cout << "En passant target: " << char('a' + f) << (r + 1) << "\n";
    }
    std::cout << "Halfmove clock: " << halfmoveClock << "\n";
    std::cout << "Fullmove number: " << fullmoveNumber << "\n";
}

// Applies a move in algebraic notation (e.g., "e2e4")
// Returns true if the move was made, false if the move is invalid or illegal
bool Board::makeMove(const std::string& move) {
    int from, to;
    if (!parseMove(move, from, to)) {
        std::cout << "Invalid move format: " << move << "\n";
        return false;
    }
    // Basic legality: must move a piece of the correct colour
    Square& source = squares[from];
    Square& destination = squares[to];
    if (source.colour != sideToMove || source.piece == EMPTY) {
        std::cout << "No " << (sideToMove == WHITE ? "White" : "Black") << " piece on source square.\n";
        return false;
    }
    // TODO: Add full legality checks (once movegen is implemented)
    // For now, allow any piece to move to any destination (simple model)
    destination = source; // Move piece
    source = Square();    // Empty source
    // Update game state
    sideToMove = (sideToMove == WHITE ? BLACK : WHITE);
    halfmoveClock++;
    if (sideToMove == WHITE) fullmoveNumber++;
    // En passant, castling, and promotion not handled yet
    return true;
}

// Checks if the game is over (very basic placeholder)
bool Board::isGameOver() const {
    // For now, always returns false
    // Later: check for checkmate, stalemate, insufficient material, etc.
    return false;
}

// Returns the side to move
Board::Colour Board::getSideToMove() const {
    return sideToMove;
}

// Returns the piece and colour on a given square index
Board::Square Board::getSquare(int index) const {
    if (index < 0 || index >= NUM_SQUARES)
        return Square();
    return squares[index];
}

// Utility: converts file (0-7), rank (0-7) to square index (0-63)
int Board::toIndex(int file, int rank) {
    return rank * BOARD_SIZE + file;
}

// Returns a FEN string representing the board position
std::string Board::getFEN() const {
    std::ostringstream fen;
    // Piece placement
    for (int r = BOARD_SIZE - 1; r >= 0; --r) {
        int emptyCount = 0;
        for (int f = 0; f < BOARD_SIZE; ++f) {
            const Square& sq = squares[toIndex(f, r)];
            if (sq.piece == EMPTY) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen << emptyCount;
                    emptyCount = 0;
                }
                char symbol = '.';
                switch (sq.piece) {
                    case PAWN:   symbol = 'P'; break;
                    case KNIGHT: symbol = 'N'; break;
                    case BISHOP: symbol = 'B'; break;
                    case ROOK:   symbol = 'R'; break;
                    case QUEEN:  symbol = 'Q'; break;
                    case KING:   symbol = 'K'; break;
                    default:     symbol = '?';
                }
                if (sq.colour == BLACK)
                    symbol = std::tolower(symbol);
                fen << symbol;
            }
        }
        if (emptyCount > 0) fen << emptyCount;
        if (r > 0) fen << '/';
    }
    // Side to move
    fen << ' ' << (sideToMove == WHITE ? 'w' : 'b');
    // Castling rights
    std::string castling;
    if (castlingRights[0]) castling += 'K';
    if (castlingRights[1]) castling += 'Q';
    if (castlingRights[2]) castling += 'k';
    if (castlingRights[3]) castling += 'q';
    fen << ' ' << (castling.empty() ? "-" : castling);
    // En passant
    if (enPassantSquare == -1) {
        fen << " -";
    } else {
        int f = enPassantSquare % BOARD_SIZE;
        int r = enPassantSquare / BOARD_SIZE;
        fen << ' ' << char('a' + f) << (r + 1);
    }
    // Halfmove clock and fullmove number
    fen << ' ' << halfmoveClock << ' ' << fullmoveNumber;
    return fen.str();
}

// Parses a move string in algebraic notation ("e2e4") into from/to indices
bool Board::parseMove(const std::string& move, int& from, int& to) const {
    if (move.length() < 4) return false;
    char fromFile = move[0];
    char fromRank = move[1];
    char toFile = move[2];
    char toRank = move[3];
    if (fromFile < 'a' || fromFile > 'h' || toFile < 'a' || toFile > 'h') return false;
    if (fromRank < '1' || fromRank > '8' || toRank < '1' || toRank > '8') return false;
    int ff = fromFile - 'a';
    int fr = fromRank - '1';
    int tf = toFile - 'a';
    int tr = toRank - '1';
    from = toIndex(ff, fr);
    to = toIndex(tf, tr);
    return true;
}
