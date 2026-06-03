#include "bot_lv2.h"
#include <algorithm>
#include <vector>
#include "../../utils/logger.h"
#include "../logic_bot.h"

BotLevel2::BotLevel2(const BotLevel& _level, const char& _symbol)
    : Bot(_level, _symbol) {}

BotLevel2::~BotLevel2() {}

pII BotLevel2::center_pick(char board[][BOARD_N_MAX], const int size) {
    int center = size / 2;
    pII best = {-1, -1};
    int bestDist = size * 2;

    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (board[r][c] == '.') {
                int dist = std::max(std::abs(r - center), std::abs(c - center));
                if (dist < bestDist) {
                    bestDist = dist;
                    best = {r, c};
                }
            }
    return best;
}

pII BotLevel2::simple_heuristic(char board[][BOARD_N_MAX], const int size,
                                 const int goal, const char botSym,
                                 const char playerSym) {
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (LogicBot::isValidMove(board, size, r, c)) {
                board[r][c] = botSym;
                bool win = LogicBot::checkWin(board, size, botSym, goal);
                board[r][c] = '.';
                if (win) return {r, c};
            }
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (LogicBot::isValidMove(board, size, r, c)) {
                board[r][c] = playerSym;
                bool win = LogicBot::checkWin(board, size, playerSym, goal);
                board[r][c] = '.';
                if (win) return {r, c};
            }
    pII c = center_pick(board, size);
    if (c.first != -1) return c;

    return {-1, -1};
}

pII BotLevel2::getMove(char board[][BOARD_N_MAX], const int size, const int goal) {
    Logger::log("BotLevel2::getMove (heuristic+center)", Logger::Level::DEBUG);
    return simple_heuristic(board, size, goal, symbol, op_symbol);
}