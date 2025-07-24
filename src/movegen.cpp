#include "movegen.h"
#include <cassert>
#include <cctype>

// Movement offsets for each piece type
const int knightOffsets[8][2] = {
    {1, 2}, {2, 1}, {2, -1}, {1, -2},
    {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
};
const int bishopOffsets[4][2] = {
    {1, 1}, {1, -1}, {-1, -1}, {-1, 1}
};
const int rookOffsets[4][2] = {
    {1, 0}, {0, 1}, {-1, 0}, {0, -1}
};
const int kingOffsets[8][2] = {
    {1, 1}, {1, 0}, {1, -1}, {0, -1},
    {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}
};

// Generate all pseudo-legal moves for the current board position.
std::vector<Board::Move> MoveGen::generatePseudoLegalMoves(const Board& board) {
    std::vector<Board::Move> moves;
    Board::Colour side = board.getSideToMove();

    // Iterate through all squares to find pieces of the current side.
    for (int sq = 0; sq < Board::NUM_SQUARES; ++sq) {
        Board::Square square = board.getSquare(sq);
        if (square.colour != side || square.piece == Board::EMPTY)
            continue;

        // Call the appropriate helper for each piece type.
        switch (square.piece) {
            case Board::PAWN:
                addPawnMoves(board, sq, moves);
                break;
            case Board::KNIGHT:
                addKnightMoves(board, sq, moves);
                break;
            case Board::BISHOP:
                addBishopMoves(board, sq, moves);
                break;
            case Board::ROOK:
                addRookMoves(board, sq, moves);
                break;
            case Board::QUEEN:
                addQueenMoves(board, sq, moves);
                break;
            case Board::KING:
                addKingMoves(board, sq, moves);
                break;
            default:
                break;
        }
    }
    // Add castling moves (if permitted by board state)
    addCastlingMoves(board, moves);

    // Add en passant moves (if available)
    addEnPassantMoves(board, moves);

    return moves;
}

// Generate only legal moves (do not leave own king in check).
std::vector<Board::Move> MoveGen::generateLegalMoves(const Board& board) {
    std::vector<Board::Move> pseudoMoves = generatePseudoLegalMoves(board);
    std::vector<Board::Move> legalMoves;

    for (const auto& move : pseudoMoves) {
        // Make a copy of the board and apply the move.
        Board testBoard = board;
        if (!testBoard.makeMove(move)) continue;

        // Find king's location after the move.
        int kingSq = findKingSquare(testBoard, board.getSideToMove());
        Board::Colour opponent = (board.getSideToMove() == Board::WHITE) ? Board::BLACK : Board::WHITE;

        // If king is not attacked, the move is legal.
        if (kingSq != -1 && !isSquareAttacked(testBoard, kingSq, opponent))
            legalMoves.push_back(move);
    }
    return legalMoves;
}

// Converts a Move structure to algebraic notation ("e2e4", "e7e8q", etc.).
std::string MoveGen::moveToString(const Board::Move& move) {
    int fromFile, fromRank, toFile, toRank;
    Board::indexToCoords(move.from, fromFile, fromRank);
    Board::indexToCoords(move.to, toFile, toRank);

    std::string s;
    s += char('a' + fromFile);
    s += char('1' + fromRank);
    s += char('a' + toFile);
    s += char('1' + toRank);
    if (move.promotion != Board::EMPTY) {
        switch (move.promotion) {
            case Board::QUEEN:  s += 'q'; break;
            case Board::ROOK:   s += 'r'; break;
            case Board::BISHOP: s += 'b'; break;
            case Board::KNIGHT: s += 'n'; break;
            default: break;
        }
    }
    return s;
}

// Checks if a move is legal in the current position.
bool MoveGen::isLegalMove(const Board& board, const Board::Move& move) {
    auto legalMoves = generateLegalMoves(board);
    for (const auto& m : legalMoves) {
        if (m.from == move.from && m.to == move.to && m.promotion == move.promotion &&
            m.isCastle == move.isCastle && m.isEnPassant == move.isEnPassant)
            return true;
    }
    return false;
}

// Helper: Generates all pawn moves (including promotions and double advances).
void MoveGen::addPawnMoves(const Board& board, int from, std::vector<Board::Move>& moves) {
    Board::Square pawn = board.getSquare(from);
    int file, rank;
    Board::indexToCoords(from, file, rank);

    int direction = (pawn.colour == Board::WHITE) ? 1 : -1;
    int startRank = (pawn.colour == Board::WHITE) ? 1 : 6;
    int promotionRank = (pawn.colour == Board::WHITE) ? 7 : 0;

    // Single square forward move
    int fwdRank = rank + direction;
    if (fwdRank >= 0 && fwdRank < Board::BOARD_SIZE) {
        int to = Board::toIndex(file, fwdRank);
        if (board.getSquare(to).piece == Board::EMPTY) {
            // Promotion
            if (fwdRank == promotionRank) {
                moves.emplace_back(from, to, Board::QUEEN);
                moves.emplace_back(from, to, Board::ROOK);
                moves.emplace_back(from, to, Board::BISHOP);
                moves.emplace_back(from, to, Board::KNIGHT);
            } else {
                moves.emplace_back(from, to);
            }
            // Double move from starting rank
            if (rank == startRank) {
                int dblRank = rank + 2 * direction;
                int dblTo = Board::toIndex(file, dblRank);
                if (board.getSquare(dblTo).piece == Board::EMPTY)
                    moves.emplace_back(from, dblTo);
            }
        }
    }
    // Captures to the left and right (including promotion)
    for (int df = -1; df <= 1; df += 2) {
        int capFile = file + df;
        int capRank = rank + direction;
        if (capFile >= 0 && capFile < Board::BOARD_SIZE && capRank >= 0 && capRank < Board::BOARD_SIZE) {
            int to = Board::toIndex(capFile, capRank);
            Board::Square target = board.getSquare(to);
            if (target.piece != Board::EMPTY && target.colour != pawn.colour) {
                // Promotion on capture
                if (capRank == promotionRank) {
                    moves.emplace_back(from, to, Board::QUEEN);
                    moves.emplace_back(from, to, Board::ROOK);
                    moves.emplace_back(from, to, Board::BISHOP);
                    moves.emplace_back(from, to, Board::KNIGHT);
                } else {
                    moves.emplace_back(from, to);
                }
            }
        }
    }
    // En passant handled separately
}

// Helper: Generates all knight moves (L-shaped jumps).
void MoveGen::addKnightMoves(const Board& board, int from, std::vector<Board::Move>& moves) {
    int file, rank;
    Board::indexToCoords(from, file, rank);
    Board::Square knight = board.getSquare(from);

    for (const auto& offset : knightOffsets) {
        int toFile = file + offset[0];
        int toRank = rank + offset[1];
        if (toFile >= 0 && toFile < Board::BOARD_SIZE && toRank >= 0 && toRank < Board::BOARD_SIZE) {
            int to = Board::toIndex(toFile, toRank);
            Board::Square target = board.getSquare(to);
            if (target.piece == Board::EMPTY || target.colour != knight.colour) {
                moves.emplace_back(from, to);
            }
        }
    }
}

// Helper: Generates all bishop moves (diagonals).
void MoveGen::addBishopMoves(const Board& board, int from, std::vector<Board::Move>& moves) {
    int file, rank;
    Board::indexToCoords(from, file, rank);
    Board::Square bishop = board.getSquare(from);

    for (const auto& offset : bishopOffsets) {
        for (int dist = 1; dist < Board::BOARD_SIZE; ++dist) {
            int toFile = file + offset[0] * dist;
            int toRank = rank + offset[1] * dist;
            if (toFile < 0 || toFile >= Board::BOARD_SIZE || toRank < 0 || toRank >= Board::BOARD_SIZE)
                break;
            int to = Board::toIndex(toFile, toRank);
            Board::Square target = board.getSquare(to);
            if (target.piece == Board::EMPTY) {
                moves.emplace_back(from, to);
            } else {
                if (target.colour != bishop.colour)
                    moves.emplace_back(from, to);
                break; // Blocked by any piece
            }
        }
    }
}

// Helper: Generates all rook moves (straight lines).
void MoveGen::addRookMoves(const Board& board, int from, std::vector<Board::Move>& moves) {
    int file, rank;
    Board::indexToCoords(from, file, rank);
    Board::Square rook = board.getSquare(from);

    for (const auto& offset : rookOffsets) {
        for (int dist = 1; dist < Board::BOARD_SIZE; ++dist) {
            int toFile = file + offset[0] * dist;
            int toRank = rank + offset[1] * dist;
            if (toFile < 0 || toFile >= Board::BOARD_SIZE || toRank < 0 || toRank >= Board::BOARD_SIZE)
                break;
            int to = Board::toIndex(toFile, toRank);
            Board::Square target = board.getSquare(to);
            if (target.piece == Board::EMPTY) {
                moves.emplace_back(from, to);
            } else {
                if (target.colour != rook.colour)
                    moves.emplace_back(from, to);
                break; // Blocked by any piece
            }
        }
    }
}

// Helper: Generates all queen moves (combines rook and bishop).
void MoveGen::addQueenMoves(const Board& board, int from, std::vector<Board::Move>& moves) {
    addBishopMoves(board, from, moves);
    addRookMoves(board, from, moves);
}

// Helper: Generates all king moves (one square in any direction).
void MoveGen::addKingMoves(const Board& board, int from, std::vector<Board::Move>& moves) {
    int file, rank;
    Board::indexToCoords(from, file, rank);
    Board::Square king = board.getSquare(from);

    for (const auto& offset : kingOffsets) {
        int toFile = file + offset[0];
        int toRank = rank + offset[1];
        if (toFile >= 0 && toFile < Board::BOARD_SIZE && toRank >= 0 && toRank < Board::BOARD_SIZE) {
            int to = Board::toIndex(toFile, toRank);
            Board::Square target = board.getSquare(to);
            if (target.piece == Board::EMPTY || target.colour != king.colour) {
                moves.emplace_back(from, to);
            }
        }
    }
    // Castling is handled separately
}

// Helper: Adds castling moves if the current side has rights and the squares are clear/not attacked.
void MoveGen::addCastlingMoves(const Board& board, std::vector<Board::Move>& moves) {
    Board::Colour side = board.getSideToMove();
    int rank = (side == Board::WHITE) ? 0 : 7;
    int kingFrom = Board::toIndex(4, rank);

    auto rights = board.getCastlingRights();
    Board::Square king = board.getSquare(kingFrom);

    // Kingside castling
    if ((side == Board::WHITE ? rights[0] : rights[2]) &&
        board.getSquare(Board::toIndex(5, rank)).piece == Board::EMPTY &&
        board.getSquare(Board::toIndex(6, rank)).piece == Board::EMPTY) {

        // For 1500 Elo, basic check: king not in check, squares not attacked
        if (!isSquareAttacked(board, kingFrom, side == Board::WHITE ? Board::BLACK : Board::WHITE) &&
            !isSquareAttacked(board, Board::toIndex(5, rank), side == Board::WHITE ? Board::BLACK : Board::WHITE) &&
            !isSquareAttacked(board, Board::toIndex(6, rank), side == Board::WHITE ? Board::BLACK : Board::WHITE))
            moves.emplace_back(kingFrom, Board::toIndex(6, rank), Board::EMPTY, true, false);
    }
    // Queenside castling
    if ((side == Board::WHITE ? rights[1] : rights[3]) &&
        board.getSquare(Board::toIndex(1, rank)).piece == Board::EMPTY &&
        board.getSquare(Board::toIndex(2, rank)).piece == Board::EMPTY &&
        board.getSquare(Board::toIndex(3, rank)).piece == Board::EMPTY) {

        if (!isSquareAttacked(board, kingFrom, side == Board::WHITE ? Board::BLACK : Board::WHITE) &&
            !isSquareAttacked(board, Board::toIndex(3, rank), side == Board::WHITE ? Board::BLACK : Board::WHITE) &&
            !isSquareAttacked(board, Board::toIndex(2, rank), side == Board::WHITE ? Board::BLACK : Board::WHITE))
            moves.emplace_back(kingFrom, Board::toIndex(2, rank), Board::EMPTY, true, false);
    }
}

// Helper: Adds en passant moves if available.
void MoveGen::addEnPassantMoves(const Board& board, std::vector<Board::Move>& moves) {
    int epSq = board.getEnPassantSquare();
    if (epSq == -1) return;

    Board::Colour side = board.getSideToMove();
    int file, rank;
    Board::indexToCoords(epSq, file, rank);

    // Only pawns on the correct rank can capture en passant
    int pawnRank = (side == Board::WHITE) ? rank - 1 : rank + 1;
    if (pawnRank < 0 || pawnRank >= Board::BOARD_SIZE) return;

    // Check left and right files
    for (int df = -1; df <= 1; df += 2) {
        int pawnFile = file + df;
        if (pawnFile < 0 || pawnFile >= Board::BOARD_SIZE) continue;
        int from = Board::toIndex(pawnFile, pawnRank);
        Board::Square pawn = board.getSquare(from);
        if (pawn.piece == Board::PAWN && pawn.colour == side) {
            moves.emplace_back(from, epSq, Board::EMPTY, false, true);
        }
    }
}

// Utility: Checks if a given square is attacked by the opponent.
// This is a simplified version; more advanced logic can be added for higher Elo.
bool MoveGen::isSquareAttacked(const Board& board, int square, Board::Colour attacker) {
    // For each piece of the attacker, generate its attacks and see if any hit the square.
    for (int sq = 0; sq < Board::NUM_SQUARES; ++sq) {
        Board::Square piece = board.getSquare(sq);
        if (piece.colour != attacker || piece.piece == Board::EMPTY)
            continue;
        std::vector<Board::Move> tempMoves;
        switch (piece.piece) {
            case Board::PAWN: {
                int file, rank;
                Board::indexToCoords(sq, file, rank);
                int direction = (attacker == Board::WHITE) ? 1 : -1;
                for (int df = -1; df <= 1; df += 2) {
                    int capFile = file + df;
                    int capRank = rank + direction;
                    if (capFile >= 0 && capFile < Board::BOARD_SIZE && capRank >= 0 && capRank < Board::BOARD_SIZE) {
                        int to = Board::toIndex(capFile, capRank);
                        if (to == square)
                            return true;
                    }
                }
                break;
            }
            case Board::KNIGHT: {
                int file, rank;
                Board::indexToCoords(sq, file, rank);
                for (const auto& offset : knightOffsets) {
                    int toFile = file + offset[0];
                    int toRank = rank + offset[1];
                    if (toFile >= 0 && toFile < Board::BOARD_SIZE && toRank >= 0 && toRank < Board::BOARD_SIZE) {
                        int to = Board::toIndex(toFile, toRank);
                        if (to == square)
                            return true;
                    }
                }
                break;
            }
            case Board::BISHOP: {
                int file, rank;
                Board::indexToCoords(sq, file, rank);
                for (const auto& offset : bishopOffsets) {
                    for (int dist = 1; dist < Board::BOARD_SIZE; ++dist) {
                        int toFile = file + offset[0] * dist;
                        int toRank = rank + offset[1] * dist;
                        if (toFile < 0 || toFile >= Board::BOARD_SIZE || toRank < 0 || toRank >= Board::BOARD_SIZE)
                            break;
                        int to = Board::toIndex(toFile, toRank);
                        if (to == square)
                            return true;
                        if (board.getSquare(to).piece != Board::EMPTY)
                            break;
                    }
                }
                break;
            }
            case Board::ROOK: {
                int file, rank;
                Board::indexToCoords(sq, file, rank);
                for (const auto& offset : rookOffsets) {
                    for (int dist = 1; dist < Board::BOARD_SIZE; ++dist) {
                        int toFile = file + offset[0] * dist;
                        int toRank = rank + offset[1] * dist;
                        if (toFile < 0 || toFile >= Board::BOARD_SIZE || toRank < 0 || toRank >= Board::BOARD_SIZE)
                            break;
                        int to = Board::toIndex(toFile, toRank);
                        if (to == square)
                            return true;
                        if (board.getSquare(to).piece != Board::EMPTY)
                            break;
                    }
                }
                break;
            }
            case Board::QUEEN: {
                int file, rank;
                Board::indexToCoords(sq, file, rank);
                // Bishop directions
                for (const auto& offset : bishopOffsets) {
                    for (int dist = 1; dist < Board::BOARD_SIZE; ++dist) {
                        int toFile = file + offset[0] * dist;
                        int toRank = rank + offset[1] * dist;
                        if (toFile < 0 || toFile >= Board::BOARD_SIZE || toRank < 0 || toRank >= Board::BOARD_SIZE)
                            break;
                        int to = Board::toIndex(toFile, toRank);
                        if (to == square)
                            return true;
                        if (board.getSquare(to).piece != Board::EMPTY)
                            break;
                    }
                }
                // Rook directions
                for (const auto& offset : rookOffsets) {
                    for (int dist = 1; dist < Board::BOARD_SIZE; ++dist) {
                        int toFile = file + offset[0] * dist;
                        int toRank = rank + offset[1] * dist;
                        if (toFile < 0 || toFile >= Board::BOARD_SIZE || toRank < 0 || toRank >= Board::BOARD_SIZE)
                            break;
                        int to = Board::toIndex(toFile, toRank);
                        if (to == square)
                            return true;
                        if (board.getSquare(to).piece != Board::EMPTY)
                            break;
                    }
                }
                break;
            }
            case Board::KING: {
                int file, rank;
                Board::indexToCoords(sq, file, rank);
                for (const auto& offset : kingOffsets) {
                    int toFile = file + offset[0];
                    int toRank = rank + offset[1];
                    if (toFile >= 0 && toFile < Board::BOARD_SIZE && toRank >= 0 && toRank < Board::BOARD_SIZE) {
                        int to = Board::toIndex(toFile, toRank);
                        if (to == square)
                            return true;
                    }
                }
                break;
            }
            default: break;
        }
    }
    return false;
}

// Utility: Finds the square index of the king for a given colour.
int MoveGen::findKingSquare(const Board& board, Board::Colour colour) {
    for (int sq = 0; sq < Board::NUM_SQUARES; ++sq) {
        Board::Square piece = board.getSquare(sq);
        if (piece.piece == Board::KING && piece.colour == colour)
            return sq;
    }
    return -1;
}
