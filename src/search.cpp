#include "search.h"
#include <algorithm>
#include <iostream>

// Finds the best move for the current position at the given search depth.
// Returns the best move and its evaluation score via outScore.
Board::Move Search::findBestMove(const Board& board, int depth, int& outScore) {
    Board::Move bestMove(0, 0);
    int bestScore = std::numeric_limits<int>::min();

    // Generate all legal moves for the side to move.
    auto moves = MoveGen::generateLegalMoves(board);
    if (moves.empty()) {
        outScore = Evaluate::score(board);
        return bestMove;
    }

    // Order moves (captures first, then others) for efficiency.
    moves = orderMoves(board, moves);

    // Try all moves and choose the one with the highest score.
    for (const auto& move : moves) {
        Board boardCopy = board;
        if (!boardCopy.makeMove(move)) continue;

        int score = minimax(boardCopy, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    outScore = bestScore;
    return bestMove;
}

// Convenience overload: returns only the best move.
Board::Move Search::findBestMove(const Board& board, int depth) {
    int dummyScore;
    return findBestMove(board, depth, dummyScore);
}

// Core minimax search with alpha-beta pruning.
// Maximising for White, minimising for Black.
int Search::minimax(Board& board, int depth, int alpha, int beta, bool maximisingPlayer) {
    // Base case: leaf node (depth 0) or game over.
    if (depth == 0 || board.isGameOver()) {
        return Evaluate::score(board);
    }

    auto moves = MoveGen::generateLegalMoves(board);
    if (moves.empty()) {
        // No legal moves: checkmate or stalemate.
        return checkGameOver(board, depth);
    }

    // Order moves for efficiency.
    moves = orderMoves(board, moves);

    if (maximisingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            Board boardCopy = board;
            if (!boardCopy.makeMove(move)) continue;
            int eval = minimax(boardCopy, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break; // Beta cut-off
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            Board boardCopy = board;
            if (!boardCopy.makeMove(move)) continue;
            int eval = minimax(boardCopy, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break; // Alpha cut-off
        }
        return minEval;
    }
}

// Orders moves for search efficiency (MVV/LVA: Most Valuable Victim / Least Valuable Attacker).
// Captures and promotions are prioritised.
std::vector<Board::Move> Search::orderMoves(const Board& board, const std::vector<Board::Move>& moves) {
    std::vector<std::pair<int, Board::Move>> scoredMoves;

    for (const auto& move : moves) {
        int score = 0;
        Board::Square target = board.getSquare(move.to);
        if (target.piece != Board::EMPTY) {
            // Capture: prioritise based on value of captured piece.
            score += Evaluate::getMaterialValue(target.piece) * 10;
            score -= Evaluate::getMaterialValue(board.getSquare(move.from).piece);
        }
        if (move.promotion != Board::EMPTY) {
            // Promotions are highly valuable.
            score += 900;
        }
        if (move.isCastle) {
            // Castling is a positive move.
            score += 50;
        }
        if (move.isEnPassant) {
            score += 100;
        }
        scoredMoves.emplace_back(score, move);
    }

    // Sort moves by descending score.
    std::sort(scoredMoves.begin(), scoredMoves.end(),
              [](const std::pair<int, Board::Move>& a, const std::pair<int, Board::Move>& b) {
                  return a.first > b.first;
              });

    // Return moves in sorted order.
    std::vector<Board::Move> ordered;
    for (const auto& p : scoredMoves) {
        ordered.push_back(p.second);
    }
    return ordered;
}

// Returns mate or stalemate scores for game-over states.
int Search::checkGameOver(const Board& board, int ply) {
    // For now, simply return a large negative value for checkmate, zero for stalemate.
    // This can be improved for more nuanced behaviour.
    auto moves = MoveGen::generateLegalMoves(board);
    if (moves.empty()) {
        // If king is in check, it's mate.
        Board::Colour opponent = (board.getSideToMove() == Board::WHITE) ? Board::BLACK : Board::WHITE;
        int kingSq = MoveGen::findKingSquare(board, board.getSideToMove());
        if (MoveGen::isSquareAttacked(board, kingSq, opponent)) {
            // Mate: losing side gets negative score, positive for winning side.
            int mateScore = -100000 + ply;
            return mateScore;
        } else {
            // Stalemate: draw.
            return 0;
        }
    }
    return Evaluate::score(board);
}
