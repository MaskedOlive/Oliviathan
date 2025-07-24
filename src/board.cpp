#include "board.h"
#include <iostream>
#include <sstream>
#include <cctype>

// Constructor: set up a fresh board
Board::Board()
    : squares(), sideToMove(WHITE), castlingRights{true, true, true, true},
      enPassantSquare(-1), halfmoveClock(0), fullmoveNumber(1)
{
    reset();
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

// Initialises pieces in their starting positions
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
    // Place major pieces for both sides
    std::array<Piece, BOARD_SIZE> backRank = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    for (int f = 0; f < BOARD_SIZE; ++f) {
        squares[toIndex(f, 0)] = Square(backRank[f], WHITE);
        squares[toIndex(f, 7)] = Square(backRank[f], BLACK);
    }
}

// Prints a textual representation of the board
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
    std::cout << "Castling rights: "
              << (castlingRights[0] ? "K" : "")
              << (castlingRights[1] ? "Q" : "")
              << (castlingRights[2] ? "k" : "")
              << (castlingRights[3] ? "q" : "") << "\n";
    if (enPassantSquare != -1) {
        int f, r;
        indexToCoords(enPassantSquare, f, r);
        std::cout << "En passant target: " << char('a' + f) << (r + 1) << "\n";
    }
    std::cout << "Halfmove clock: " << halfmoveClock << "\n";
    std::cout << "Fullmove number: " << fullmoveNumber << "\n";
}

// Applies a move in Move struct form
bool Board::makeMove(const Move& move) {
    Square& source = squares[move.from];
    Square& destination = squares[move.to];

    // Check if source piece matches side to move
    if (source.colour != sideToMove || source.piece == EMPTY) {
        std::cout << "No " << (sideToMove == WHITE ? "White" : "Black") << " piece on source square.\n";
        return false;
    }

    // Handle castling moves
    if (move.isCastle) {
        if (!isLegalCastle(move)) {
            std::cout << "Illegal castling move.\n";
            return false;
        }
        // Kingside or queenside
        int kingFrom = move.from;
        int kingTo = move.to;
        int rookFrom, rookTo;
        if (kingTo > kingFrom) { // Kingside
            rookFrom = kingFrom + 3;
            rookTo = kingFrom + 1;
        } else { // Queenside
            rookFrom = kingFrom - 4;
            rookTo = kingFrom - 1;
        }
        squares[rookTo] = squares[rookFrom]; // Move rook
        squares[rookFrom] = Square();
        destination = source; // Move king
        source = Square();
        updateCastlingRights(move);
        clearEnPassant();
        sideToMove = (sideToMove == WHITE ? BLACK : WHITE);
        if (sideToMove == WHITE) fullmoveNumber++;
        halfmoveClock++;
        return true;
    }

    // Handle en passant
    if (move.isEnPassant) {
        if (!isLegalEnPassant(move)) {
            std::cout << "Illegal en passant move.\n";
            return false;
        }
        destination = source; // Move pawn
        source = Square();
        // Remove captured pawn
        int epCaptureSq = move.to + (sideToMove == WHITE ? -BOARD_SIZE : BOARD_SIZE);
        squares[epCaptureSq] = Square();
        clearEnPassant();
        updateCastlingRights(move);
        sideToMove = (sideToMove == WHITE ? BLACK : WHITE);
        if (sideToMove == WHITE) fullmoveNumber++;
        halfmoveClock = 0; // Reset halfmove clock for capture
        return true;
    }

    // Standard moves
    // Handle pawn double advance for en passant
    if (source.piece == PAWN && std::abs(move.to - move.from) == 2 * BOARD_SIZE) {
        enPassantSquare = (move.from + move.to) / 2;
    } else {
        clearEnPassant();
    }

    // Handle promotion
    if (move.promotion != EMPTY) {
        destination = Square(move.promotion, source.colour);
    } else {
        destination = source;
    }
    // If move is a capture or pawn move, reset halfmove clock
    if (destination.piece != EMPTY || source.piece == PAWN)
        halfmoveClock = 0;
    else
        halfmoveClock++;

    source = Square(); // Empty source square
    updateCastlingRights(move);
    sideToMove = (sideToMove == WHITE ? BLACK : WHITE);
    if (sideToMove == WHITE) fullmoveNumber++;
    return true;
}

// Applies a move given in algebraic notation ("e2e4", etc.)
bool Board::makeMove(const std::string& moveStr) {
    Move move(0, 0);
    if (!parseMove(moveStr, move)) {
        std::cout << "Invalid move format: " << moveStr << "\n";
        return false;
    }
    return makeMove(move);
}

// Checks if the game is over (checkmate, stalemate, etc.)
bool Board::isGameOver() const {
    // For now, always returns false.
    // To improve strength, add checkmate/stalemate/insufficient material detection here.
    return false;
}

// Returns side to move
Board::Colour Board::getSideToMove() const {
    return sideToMove;
}

// Returns FEN string for the position
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
        int f, r;
        indexToCoords(enPassantSquare, f, r);
        fen << ' ' << char('a' + f) << (r + 1);
    }
    fen << ' ' << halfmoveClock << ' ' << fullmoveNumber;
    return fen.str();
}

// Returns piece/colour at given square
Board::Square Board::getSquare(int index) const {
    if (index < 0 || index >= NUM_SQUARES)
        return Square();
    return squares[index];
}

// Converts file/rank to square index
int Board::toIndex(int file, int rank) {
    return rank * BOARD_SIZE + file;
}

// Converts square index to file/rank
void Board::indexToCoords(int index, int& file, int& rank) {
    file = index % BOARD_SIZE;
    rank = index / BOARD_SIZE;
}

// Get castling rights
std::array<bool, 4> Board::getCastlingRights() const {
    return castlingRights;
}

// Get en passant target square
int Board::getEnPassantSquare() const {
    return enPassantSquare;
}

// Helper: clears en passant square unless just set
void Board::clearEnPassant() {
    enPassantSquare = -1;
}

// Helper: updates castling rights if king or rook moves
void Board::updateCastlingRights(const Move& move) {
    // White king moves
    if (move.from == toIndex(4, 0)) {
        castlingRights[0] = false; // White kingside
        castlingRights[1] = false; // White queenside
    }
    // Black king moves
    if (move.from == toIndex(4, 7)) {
        castlingRights[2] = false; // Black kingside
        castlingRights[3] = false; // Black queenside
    }
    // White rook moves
    if (move.from == toIndex(0, 0)) castlingRights[1] = false; // White queenside
    if (move.from == toIndex(7, 0)) castlingRights[0] = false; // White kingside
    // Black rook moves
    if (move.from == toIndex(0, 7)) castlingRights[3] = false; // Black queenside
    if (move.from == toIndex(7, 7)) castlingRights[2] = false; // Black kingside
    // If rook is captured
    if (move.to == toIndex(0, 0) && squares[move.to].piece == ROOK) castlingRights[1] = false;
    if (move.to == toIndex(7, 0) && squares[move.to].piece == ROOK) castlingRights[0] = false;
    if (move.to == toIndex(0, 7) && squares[move.to].piece == ROOK) castlingRights[3] = false;
    if (move.to == toIndex(7, 7) && squares[move.to].piece == ROOK) castlingRights[2] = false;
}

// Helper: parses algebraic move notation "e2e4", "e7e8q", etc.
bool Board::parseMove(const std::string& moveStr, Move& move) const {
    if (moveStr.length() < 4) return false;
    // Parse from/to squares
    char fromFile = moveStr[0];
    char fromRank = moveStr[1];
    char toFile = moveStr[2];
    char toRank = moveStr[3];
    if (fromFile < 'a' || fromFile > 'h' || toFile < 'a' || toFile > 'h') return false;
    if (fromRank < '1' || fromRank > '8' || toRank < '1' || toRank > '8') return false;
    int ff = fromFile - 'a';
    int fr = fromRank - '1';
    int tf = toFile - 'a';
    int tr = toRank - '1';
    move.from = toIndex(ff, fr);
    move.to = toIndex(tf, tr);
    move.promotion = EMPTY;
    move.isCastle = false;
    move.isEnPassant = false;

    // Check for promotion (e.g., "e7e8q")
    if (moveStr.length() >= 5) {
        switch (std::tolower(moveStr[4])) {
            case 'q': move.promotion = QUEEN; break;
            case 'r': move.promotion = ROOK; break;
            case 'b': move.promotion = BISHOP; break;
            case 'n': move.promotion = KNIGHT; break;
            default: move.promotion = EMPTY;
        }
    }

    // Check for castling (king move two squares)
    Square src = getSquare(move.from);
    if (src.piece == KING && std::abs(tf - ff) == 2 && fr == tr) {
        move.isCastle = true;
    }

    // Check for en passant (pawn diagonal, target is empty, en passant square matches)
    if (src.piece == PAWN && squares[move.to].piece == EMPTY && ff != tf && enPassantSquare == move.to) {
        move.isEnPassant = true;
    }

    return true;
}

// Helper: checks if castling move is legal (must add further checks for king not in check and squares not attacked)
bool Board::isLegalCastle(const Move& move) const {
    // For simplicity, just check castling rights and empty squares between king and rook
    // Full legality (king not in check, squares not attacked) should be checked in movegen
    int rank = (sideToMove == WHITE) ? 0 : 7;
    if (move.to > move.from) { // Kingside
        if (!(sideToMove == WHITE ? castlingRights[0] : castlingRights[2]))
            return false;
        // Squares between king and rook must be empty
        for (int f = 5; f <= 6; ++f)
            if (getSquare(toIndex(f, rank)).piece != EMPTY)
                return false;
    } else { // Queenside
        if (!(sideToMove == WHITE ? castlingRights[1] : castlingRights[3]))
            return false;
        for (int f = 1; f <= 3; ++f)
            if (getSquare(toIndex(f, rank)).piece != EMPTY)
                return false;
    }
    return true;
}

// Helper: checks if en passant move is legal (target square matches and pawn is in correct rank)
bool Board::isLegalEnPassant(const Move& move) const {
    // Check en passant square
    return enPassantSquare == move.to;
}

// Helper: checks if a square is attacked by the given colour (basic placeholder, extend for full legality)
bool Board::isSquareAttacked(int square, Colour attacker) const {
    // For now, this is a placeholder.
    // Full implementation should be in movegen.
    return false;
}
