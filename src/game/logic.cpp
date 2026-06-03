#include "logic.h"
#include <algorithm>
#include <format>
#include <sstream>
#include <vector>
#include "../utils/logger.h"

void Logic::initBoard(char board[][BOARD_N_MAX], const int size) {
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            board[r][c] = '.';
    Logger::log(std::format("Logic::initBoard: {}x{}", size, size), Logger::Level::DEBUG);
}

bool Logic::isValidMove(const char board[][BOARD_N_MAX], const int size,
                        const int row, const int col) {
    return row >= 0 && row < size &&
           col >= 0 && col < size &&
           board[row][col] == '.';
}

void Logic::makeMove(char board[][BOARD_N_MAX], const int row, const int col,
                     const char symbol) {
    board[row][col] = symbol;
}

bool Logic::isEmptyHead(const char board[][BOARD_N_MAX], const int size,
                        int x, int y, const char symbol) {
    if (x < 0 || x >= size || y < 0 || y >= size) return true;
    if (board[x][y] == '.') return true;
    if (board[x][y] == symbol) return true;
    return false;
}

bool Logic::checkLine(const char board[][BOARD_N_MAX], const int size,
                      const char symbol, const int goal, EndRule rule,
                      int startRow, int startCol, int dr, int dc) {
    for (int r = startRow, c = startCol, k = 0; k <= size - goal;
         r += dr, c += dc, ++k) {
        bool ok = true;
        for (int step = 0; step < goal; ++step) {
            int nr = r + step * dr, nc = c + step * dc;
            if (nr < 0 || nr >= size || nc < 0 || nc >= size ||
                board[nr][nc] != symbol) { ok = false; break; }
        }
        if (!ok) continue;
        if (rule == EndRule::NONE) return true;

        int tailR = r + goal * dr, tailC = c + goal * dc;
        int headR = r - dr,        headC = c - dc;

        bool headOpen = Logic::isEmptyHead(board, size, headR, headC, symbol);
        bool tailOpen = Logic::isEmptyHead(board, size, tailR, tailC, symbol);

        if (rule == EndRule::OPEN_ONE && (headOpen || tailOpen)) return true;
        if (rule == EndRule::OPEN_TWO && headOpen && tailOpen)   return true;
    }
    return false;
}

bool Logic::checkWin(char board[][BOARD_N_MAX], const int size,
                     const char symbol, const int goal, EndRule rule) {
    for (int r = 0; r < size; ++r)
        if (checkLine(board, size, symbol, goal, rule, r, 0, 0, 1)) return true;
    for (int c = 0; c < size; ++c)
        if (checkLine(board, size, symbol, goal, rule, 0, c, 1, 0)) return true;
    for (int r = 0; r < size; ++r)
        if (checkLine(board, size, symbol, goal, rule, r, 0, 1, 1)) return true;
    for (int c = 1; c < size; ++c)
        if (checkLine(board, size, symbol, goal, rule, 0, c, 1, 1)) return true;
    for (int r = 0; r < size; ++r)
        if (checkLine(board, size, symbol, goal, rule, r, size - 1, 1, -1)) return true;
    for (int c = 0; c < size - 1; ++c)
        if (checkLine(board, size, symbol, goal, rule, 0, c, 1, -1)) return true;
    return false;
}


bool Logic::checkDraw(char board[][BOARD_N_MAX], const int size) {
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (board[r][c] == '.') return false;
    return true;
}

std::optional<WinLine> Logic::getWinLine(const char board[][BOARD_N_MAX],
                                         const int size, const char symbol,
                                         const int goal, EndRule rule) {
    auto tryLine = [&](int sr, int sc, int dr, int dc) -> std::optional<WinLine> {
        for (int r = sr, c = sc, k = 0; k <= size - goal;
             r += dr, c += dc, ++k) {
            bool ok = true;
            for (int step = 0; step < goal; ++step) {
                int nr = r + step * dr, nc = c + step * dc;
                if (nr < 0 || nr >= size || nc < 0 || nc >= size ||
                    board[nr][nc] != symbol) { ok = false; break; }
            }
            if (!ok) continue;
            if (rule != EndRule::NONE) {
                int tailR = r + goal * dr, tailC = c + goal * dc;
                int headR = r - dr,        headC = c - dc;
                bool headOpen = Logic::isEmptyHead(board, size, headR, headC, symbol);
                bool tailOpen = Logic::isEmptyHead(board, size, tailR, tailC, symbol);
                if (rule == EndRule::OPEN_ONE && !(headOpen || tailOpen)) continue;
                if (rule == EndRule::OPEN_TWO && !(headOpen && tailOpen)) continue;
            }
            WinLine wl;
            for (int step = 0; step < goal; ++step)
                wl.cells.push_back({r + step * dr, c + step * dc});
            return wl;
        }
        return std::nullopt;
    };

    for (int r = 0; r < size; ++r)
        if (auto w = tryLine(r, 0, 0, 1)) return w;
    for (int c = 0; c < size; ++c)
        if (auto w = tryLine(0, c, 1, 0)) return w;
    for (int r = 0; r < size; ++r)
        if (auto w = tryLine(r, 0, 1, 1)) return w;
    for (int c = 1; c < size; ++c)
        if (auto w = tryLine(0, c, 1, 1)) return w;
    for (int r = 0; r < size; ++r)
        if (auto w = tryLine(r, size - 1, 1, -1)) return w;
    for (int c = 0; c < size - 1; ++c)
        if (auto w = tryLine(0, c, 1, -1)) return w;
    return std::nullopt;
}
