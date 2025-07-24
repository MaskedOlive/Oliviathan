#include "uci.h"
#include <iostream>
#include <sstream>
#include <chrono>

// Splits a string into tokens using spaces (for command parsing).
std::vector<std::string> UCI::split(const std::string& s) {
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string token;
    while (iss >> token)
        tokens.push_back(token);
    return tokens;
}

// Starts the UCI command loop.
// This loop reads lines from stdin and responds to UCI protocol commands.
// You can add more commands or options for further engine features.
void UCI::run() {
    std::string line;

    std::cout << "UCI protocol handler started. Waiting for commands...\n";

    while (std::getline(std::cin, line)) {
        auto tokens = split(line);

        if (tokens.empty()) continue;

        if (tokens[0] == "uci") {
            handleUci();
        } else if (tokens[0] == "isready") {
            handleIsReady();
        } else if (tokens[0] == "ucinewgame") {
            handleUciNewGame();
        } else if (tokens[0] == "position") {
            handlePosition(tokens);
        } else if (tokens[0] == "go") {
            handleGo(tokens);
        } else if (tokens[0] == "stop") {
            handleStop();
        } else if (tokens[0] == "quit") {
            handleQuit();
            break;
        }
        // You can add more commands here (setoption, ponderhit etc.)
    }
}

// UCI "uci" command: print engine name and author.
void UCI::handleUci() {
    std::cout << "id name Oliviathan\n";
    std::cout << "id author MaskedOlive\n";
    std::cout << "uciok\n";
}

// UCI "isready" command: signal ready.
void UCI::handleIsReady() {
    std::cout << "readyok\n";
}

// UCI "ucinewgame" command: reset to starting position.
void UCI::handleUciNewGame() {
    board.reset();
}

// UCI "position" command: set up position from FEN or startpos, and apply moves.
void UCI::handlePosition(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) return;
    int idx = 1;
    if (tokens[idx] == "startpos") {
        board.reset();
        idx++;
    } else if (tokens[idx] == "fen") {
        std::ostringstream fen;
        idx++;
        for (int i = 0; i < 6 && idx < tokens.size() && tokens[idx] != "moves"; ++i, ++idx) {
            if (i > 0) fen << ' ';
            fen << tokens[idx];
        }
        // Basic FEN support (could be extended for error checking)
        // For simplicity, use reset and ignore custom FEN in this template.
        board.reset();
    }

    // Apply moves if specified.
    while (idx < tokens.size()) {
        if (tokens[idx] == "moves") {
            std::vector<std::string> moves;
            ++idx;
            while (idx < tokens.size()) {
                moves.push_back(tokens[idx]);
                ++idx;
            }
            applyMoves(moves);
            break;
        }
        ++idx;
    }
}

// Helper: Applies a list of moves in algebraic notation to the board.
void UCI::applyMoves(const std::vector<std::string>& moves) {
    for (const auto& moveStr : moves) {
        board.makeMove(moveStr);
    }
}

// UCI "go" command: initiates thinking/search.
// For demo, supports only "go depth <n>".
void UCI::handleGo(const std::vector<std::string>& tokens) {
    int depth = 4; // Default search depth
    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i] == "depth" && i + 1 < tokens.size()) {
            depth = std::stoi(tokens[i + 1]);
        }
        // You can add support for movetime, nodes, infinite, etc.
    }

    stopSignal = false;

    // Search for the best move
    auto startTime = std::chrono::steady_clock::now();

    int score = 0;
    Board::Move bestMove = Search::findBestMove(board, depth, score);

    auto endTime = std::chrono::steady_clock::now();
    int timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    // Info line (depth, score, time, nodes)
    printInfo(depth, score, timeMs, 0); // Node count could be added via perft or search stats.

    // Output best move in UCI format.
    std::cout << "bestmove " << MoveGen::moveToString(bestMove) << "\n";
}

// Prints an info line (for GUI feedback).
void UCI::printInfo(int depth, int score, int timeMs, int nodes) {
    std::cout << "info depth " << depth
              << " score cp " << score
              << " time " << timeMs
              << " nodes " << nodes << "\n";
}

// UCI "stop" command: sets stop signal for multi-threaded search (not used in this template).
void UCI::handleStop() {
    stopSignal = true;
}

// UCI "quit" command: exits the protocol handler.
void UCI::handleQuit() {
    // Any clean-up if needed.
}
