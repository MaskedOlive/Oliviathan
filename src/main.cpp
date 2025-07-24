#include <iostream>
#include <string>
#include "board.h"
#include "movegen.h"
#include "perft.h"

// Entry point for the chess engine.
// This main file sets up the engine, provides a simple command loop, and acts as a demonstration/test harness.
// It is designed to be extensible so you can add features like UCI support, evaluation, and search algorithms later.
// Detailed UK English comments are provided to help you understand and extend the code.

int main() {
    // Create the chess board and initialise to standard starting position.
    Board board;
    board.reset();

    std::cout << "Oliviathan Chess Engine (Demo)\n";
    std::cout << "Type 'help' for options.\n";

    // Simple command loop to interact with the engine.
    while (true) {
        board.display(); // Show the current board position.

        std::cout << "\n> ";
        std::string command;
        std::getline(std::cin, command);

        // Respond to user commands.
        if (command == "quit" || command == "exit") {
            std::cout << "Exiting engine. Goodbye!\n";
            break;
        } else if (command == "help") {
            std::cout << "Commands:\n";
            std::cout << "  move <algebraic>   - Make a move (e.g., e2e4, e7e8q)\n";
            std::cout << "  fen                - Show FEN of current position\n";
            std::cout << "  perft <depth>      - Run perft test to given depth\n";
            std::cout << "  reset              - Reset board to starting position\n";
            std::cout << "  quit/exit          - Exit engine\n";
        } else if (command.substr(0, 5) == "move ") {
            // Extract move string.
            std::string moveStr = command.substr(5);
            if (board.makeMove(moveStr)) {
                std::cout << "Move played: " << moveStr << "\n";
            } else {
                std::cout << "Invalid move: " << moveStr << "\n";
            }
        } else if (command == "fen") {
            std::cout << "FEN: " << board.getFEN() << "\n";
        } else if (command.substr(0, 6) == "perft ") {
            // Run perft to the specified depth.
            int depth = std::stoi(command.substr(6));
            std::cout << "Running perft to depth " << depth << "...\n";
            uint64_t nodes = Perft::run(board, depth);
            std::cout << "Perft nodes: " << nodes << "\n";
        } else if (command == "reset") {
            board.reset();
            std::cout << "Board reset to starting position.\n";
        } else if (command.empty()) {
            // Ignore empty input.
            continue;
        } else {
            std::cout << "Unknown command. Type 'help' for options.\n";
        }
    }

    return 0;
}
