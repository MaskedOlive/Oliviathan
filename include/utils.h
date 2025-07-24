#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// The Utils namespace provides small helper functions for the chess engine.
// These are designed for clarity, extensibility, and easy future upgrades.
// Thorough UK English comments are included so you can see why each function exists and how you might extend it.

namespace Utils {

    // Converts a square index (0-63) to algebraic notation ("e4", "a1", etc.)
    inline std::string indexToAlgebraic(int index) {
        // File is 0-7 and rank is 0-7
        int file = index % 8;
        int rank = index / 8;
        std::string out;
        out += char('a' + file);
        out += char('1' + rank);
        return out;
    }

    // Converts algebraic notation ("e4", "a1") to square index (0-63).
    // Returns -1 if the input is invalid.
    inline int algebraicToIndex(const std::string& str) {
        if (str.size() != 2) return -1;
        char fileChar = std::tolower(str[0]);
        char rankChar = str[1];
        if (fileChar < 'a' || fileChar > 'h') return -1;
        if (rankChar < '1' || rankChar > '8') return -1;
        int file = fileChar - 'a';
        int rank = rankChar - '1';
        return rank * 8 + file;
    }

    // Trims whitespace from both ends of a string.
    inline std::string trim(const std::string& s) {
        auto begin = s.find_first_not_of(" \t\n\r");
        auto end = s.find_last_not_of(" \t\n\r");
        if (begin == std::string::npos || end == std::string::npos) return "";
        return s.substr(begin, end - begin + 1);
    }

    // Converts a string to lowercase.
    inline std::string toLower(const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c){ return std::tolower(c); });
        return out;
    }

    // Splits a string by a delimiter and returns the parts.
    inline std::vector<std::string> split(const std::string& s, char delimiter = ' ') {
        std::vector<std::string> tokens;
        std::string token;
        for (char c : s) {
            if (c == delimiter) {
                if (!token.empty()) tokens.push_back(token);
                token.clear();
            } else {
                token += c;
            }
        }
        if (!token.empty()) tokens.push_back(token);
        return tokens;
    }

    // Checks if a string represents a valid integer.
    inline bool isInteger(const std::string& s) {
        if (s.empty()) return false;
        size_t i = 0;
        if (s[0] == '-' || s[0] == '+') ++i;
        for (; i < s.size(); ++i) {
            if (!std::isdigit(s[i])) return false;
        }
        return true;
    }

    // Converts a string to an integer. Returns 0 on failure.
    inline int toInt(const std::string& s) {
        if (!isInteger(s)) return 0;
        return std::stoi(s);
    }

    // Checks if an index is on the board (0-63).
    inline bool isOnBoard(int index) {
        return index >= 0 && index < 64;
    }

    // Checks if file and rank are valid.
    inline bool isValidSquare(int file, int rank) {
        return file >= 0 && file < 8 && rank >= 0 && rank < 8;
    }

    // Mirrors a square index vertically (used for Black's perspective in piece-square tables).
    inline int mirrorIndex(int index) {
        int file = index % 8;
        int rank = index / 8;
        int mirroredRank = 7 - rank;
        return mirroredRank * 8 + file;
    }

    // Converts a vector of integers to a comma-separated string (for debugging or output).
    inline std::string vectorToString(const std::vector<int>& vec) {
        std::string out;
        for (size_t i = 0; i < vec.size(); ++i) {
            out += std::to_string(vec[i]);
            if (i + 1 < vec.size()) out += ",";
        }
        return out;
    }

    // Converts a vector of strings to a single space-separated string.
    inline std::string join(const std::vector<std::string>& vec, const std::string& sep = " ") {
        std::string out;
        for (size_t i = 0; i < vec.size(); ++i) {
            out += vec[i];
            if (i + 1 < vec.size()) out += sep;
        }
        return out;
    }

    // For extensibility: you can add more utility functions here for logging, formatting, etc.
}
