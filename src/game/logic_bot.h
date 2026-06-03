#pragma once
#include "logic.h"   

namespace LogicBot {

inline bool isValidMove(const char board[][BOARD_N_MAX], const int size,
                        const int row, const int col) {
    return Logic::isValidMove(board, size, row, col);
}

inline bool checkWin(char board[][BOARD_N_MAX], const int size,
                     const char symbol, const int goal,
                     EndRule rule = EndRule::OPEN_TWO) {
    return Logic::checkWin(board, size, symbol, goal, rule);
}

inline bool checkDraw(char board[][BOARD_N_MAX], const int size) {
    return Logic::checkDraw(board, size);
}

}