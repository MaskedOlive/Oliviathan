#include "evaluate.h"
#include "movegen.h"
#include <algorithm>

// Piece-square tables (values in centipawns).
// White's perspective; for Black, mirror vertically.
const int Evaluate::pawnTable[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
     10, 10, 10, 10, 10, 10, 10, 10,
      5,  5,  8, 12, 12,  8,  5,  5,
      2,  2,  4, 10, 10,  4,  2,  2,
      1,  1,  2,  5,  5,  2,  1,  1,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0, -2, -2,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0
};
const int Evaluate::knightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};
const int Evaluate::bishopTable[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};
const int Evaluate::rookTable[64] = {
     0,  0,  5, 10, 10,  5,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5, 10, 10, 10, 10, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};
const int Evaluate::queenTable[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
const int Evaluate::kingTable[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

// Returns the material value assigned to a given piece type.
// These values can be tuned for improved engine strength.
int Evaluate::getMaterialValue(Board::Piece piece) {
    switch (piece) {
        case Board::PAWN:   return PAWN_VALUE;
        case Board::KNIGHT: return KNIGHT_VALUE;
        case Board::BISHOP: return BISHOP_VALUE;
        case Board::ROOK:   return ROOK_VALUE;
        case Board::QUEEN:  return QUEEN_VALUE;
        case Board::KING:   return KING_VALUE;
        default:            return 0;
    }
}

// Returns the value for a piece on a given square using its piece-square table.
// Mirrors the table for Black's perspective.
int Evaluate::getPieceSquareValue(Board::Piece piece, int square, Board::Colour colour) {
    // Mirror vertically for Black
    int idx = (colour == Board::WHITE) ? square : ((square / 8) * 8 + (7 - (square % 8)));
    switch (piece) {
        case Board::PAWN:   return pawnTable[idx];
        case Board::KNIGHT: return knightTable[idx];
        case Board::BISHOP: return bishopTable[idx];
        case Board::ROOK:   return rookTable[idx];
        case Board::QUEEN:  return queenTable[idx];
        case Board::KING:   return kingTable[idx];
        default:            return 0;
    }
}

// Returns a bonus for retaining castling rights.
int Evaluate::evaluateCastling(const Board& board) {
    int bonus = 0;
    auto rights = board.getCastlingRights();
    if (rights[0]) bonus += 20; // White kingside
    if (rights[1]) bonus += 20; // White queenside
    if (rights[2]) bonus -= 20; // Black kingside
    if (rights[3]) bonus -= 20; // Black queenside
    return bonus;
}

// Basic evaluation of doubled pawns. More advanced pawn structure analysis can be added later.
int Evaluate::evaluatePawnStructure(const Board& board) {
    int score = 0;
    // Count doubled pawns for each file and side
    for (int file = 0; file < Board::BOARD_SIZE; ++file) {
        int whitePawns = 0, blackPawns = 0;
        for (int rank = 0; rank < Board::BOARD_SIZE; ++rank) {
            int sq = Board::toIndex(file, rank);
            Board::Square s = board.getSquare(sq);
            if (s.piece == Board::PAWN) {
                if (s.colour == Board::WHITE) whitePawns++;
                else if (s.colour == Board::BLACK) blackPawns++;
            }
        }
        if (whitePawns > 1) score -= 10 * (whitePawns - 1); // Penalty for doubled white pawns
        if (blackPawns > 1) score += 10 * (blackPawns - 1); // Penalty for doubled black pawns (negative for Black)
    }
    return score;
}

// Simple mobility evaluation: number of legal moves for each side.
// Gives a small bonus for having higher mobility.
int Evaluate::evaluateMobility(const Board& board) {
    auto whiteMoves = MoveGen::generateLegalMoves(board);
    Board boardCopy = board;
    boardCopy.makeMove("a2a3"); // Dummy move to switch turn
    auto blackMoves = MoveGen::generateLegalMoves(boardCopy);
    int mobility = static_cast<int>(whiteMoves.size()) - static_cast<int>(blackMoves.size());
    return mobility;
}

// Checks if the game is over and applies large positive/negative/draw scores.
int Evaluate::evaluateGameStatus(const Board& board) {
    // Placeholder: always returns 0 for now.
    // You can implement checkmate/stalemate detection for higher strength.
    return 0;
}

// Main evaluation function: combines material, piece-square, and basic positional scores.
int Evaluate::score(const Board& board) {
    int score = 0;

    // Loop over all squares on the board.
    for (int sq = 0; sq < Board::NUM_SQUARES; ++sq) {
        Board::Square piece = board.getSquare(sq);
        if (piece.piece == Board::EMPTY) continue;
        int material = getMaterialValue(piece.piece);
        int pst = getPieceSquareValue(piece.piece, sq, piece.colour);
        // Add for White, subtract for Black.
        if (piece.colour == Board::WHITE)
            score += material + pst;
        else if (piece.colour == Board::BLACK)
            score -= material + pst;
    }

    // Add castling rights bonus.
    score += evaluateCastling(board);

    // Add pawn structure evaluation.
    score += evaluatePawnStructure(board);

    // Add mobility bonus.
    score += evaluateMobility(board);

    // Add game status (mate/stalemate) score.
    score += evaluateGameStatus(board);

    // Return total score (positive favours White, negative favours Black).
    return score;
}
