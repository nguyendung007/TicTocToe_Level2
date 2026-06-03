#pragma once
#include <optional>
#include "setup.h"

struct WinLine {
    std::vector<std::pair<int, int>> cells;
};

class Logic {
public:
    static void initBoard(char board[][BOARD_N_MAX], const int size);
    static bool isValidMove(const char board[][BOARD_N_MAX], const int size,
                            const int row, const int col);
    static void makeMove(char board[][BOARD_N_MAX], const int row,
                         const int col, const char symbol);
    static bool checkWin(char board[][BOARD_N_MAX], const int size,
                         const char symbol, const int goal,
                         EndRule rule = EndRule::OPEN_TWO);
    static bool checkDraw(char board[][BOARD_N_MAX], const int size);
    static std::optional<WinLine> getWinLine(
        const char board[][BOARD_N_MAX],
        const int size, const char symbol, const int goal,
        EndRule rule = EndRule::OPEN_TWO);

private:
    static bool isEmptyHead(const char board[][BOARD_N_MAX], const int size,
                            int x, int y, const char symbol);

    static bool checkLine(const char board[][BOARD_N_MAX], const int size,
                          const char symbol, const int goal, EndRule rule,
                          int startRow, int startCol, int dr, int dc);

    Logic()                        = delete;
    ~Logic()                       = delete;
    Logic(const Logic&)            = delete;
    Logic& operator=(const Logic&) = delete;
};