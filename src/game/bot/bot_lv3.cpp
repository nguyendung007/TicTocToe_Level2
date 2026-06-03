#include "bot_lv3.h"
#include <algorithm>
#include <climits>
#include <vector>
#include "../../utils/logger.h"
#include "../logic_bot.h"

BotLevel3::BotLevel3(const BotLevel& _level, const char& _symbol)
    : Bot(_level, _symbol) {}

BotLevel3::~BotLevel3() {}

static int countStreaks(char board[][BOARD_N_MAX], int size,
                        char sym, int len, int dr, int dc) {
    int count = 0;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c) {
            int pr = r - dr, pc = c - dc;
            if (pr >= 0 && pr < size && pc >= 0 && pc < size && board[pr][pc] == sym)
                continue;
            int streak = 0;
            int nr = r, nc = c;
            while (nr >= 0 && nr < size && nc >= 0 && nc < size
                   && board[nr][nc] == sym) {
                ++streak;
                nr += dr; nc += dc;
            }
            if (streak == len) ++count;
        }
    return count;
}

int BotLevel3::evaluate(char board[][BOARD_N_MAX], const int size,
                         const int goal, const char botSym, const char oppSym) {
    if (goal <= 1) return 0;
    int len = goal - 1;
    const int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};
    int score = 0;
    for (auto& d : dirs) {
        score += 50 * countStreaks(board, size, botSym, len, d[0], d[1]);
        score -= 50 * countStreaks(board, size, oppSym, len, d[0], d[1]);
    }
    return score;
}

std::vector<pII> BotLevel3::getCandidates(char board[][BOARD_N_MAX],
                                           const int size) {
    int radius = (size <= 8) ? 1 : 2;
    int center = size / 2;
    std::vector<std::vector<bool>> seen(size, std::vector<bool>(size, false));
    std::vector<pII> out;

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            if (board[i][j] == '.') continue;
            for (int di = -radius; di <= radius; ++di)
                for (int dj = -radius; dj <= radius; ++dj) {
                    int ni = i + di, nj = j + dj;
                    if (ni < 0 || ni >= size || nj < 0 || nj >= size) continue;
                    if (board[ni][nj] != '.' || seen[ni][nj]) continue;
                    seen[ni][nj] = true;
                    out.push_back({ni, nj});
                }
        }

    if (out.empty()) {
        out.push_back({center, center});
        return out;
    }

    std::sort(out.begin(), out.end(), [&](const pII& a, const pII& b) {
        int da = std::max(std::abs(a.first - center), std::abs(a.second - center));
        int db = std::max(std::abs(b.first - center), std::abs(b.second - center));
        return da < db;
    });

    return out;
}

int BotLevel3::minimax(char board[][BOARD_N_MAX], int size, int goal,
                        char botSym, char oppSym,
                        int depth, int alpha, int beta, bool isMax) {
    if (LogicBot::checkWin(board, size, botSym, goal)) return  10000 + depth;
    if (LogicBot::checkWin(board, size, oppSym, goal)) return -10000 - depth;
    if (LogicBot::checkDraw(board, size))              return 0;
    if (depth == 0) return evaluate(board, size, goal, botSym, oppSym);

    auto cands = getCandidates(board, size);
    char curSym = isMax ? botSym : oppSym;

    if (isMax) {
        int best = INT_MIN;
        for (auto [r, c] : cands) {
            board[r][c] = curSym;
            int val = minimax(board, size, goal, botSym, oppSym,
                              depth - 1, alpha, beta, false);
            board[r][c] = '.';
            best  = std::max(best, val);
            alpha = std::max(alpha, best);
            if (alpha >= beta) break;
        }
        return best;
    } else {
        int best = INT_MAX;
        for (auto [r, c] : cands) {
            board[r][c] = curSym;
            int val = minimax(board, size, goal, botSym, oppSym,
                              depth - 1, alpha, beta, true);
            board[r][c] = '.';
            best = std::min(best, val);
            beta = std::min(beta, best);
            if (alpha >= beta) break;
        }
        return best;
    }
}

pII BotLevel3::hard_level(char board[][BOARD_N_MAX], const int size, const int goal) {
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (LogicBot::isValidMove(board, size, r, c)) {
                board[r][c] = symbol;
                bool win = LogicBot::checkWin(board, size, symbol, goal);
                board[r][c] = '.';
                if (win) return {r, c};
            }

    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (LogicBot::isValidMove(board, size, r, c)) {
                board[r][c] = op_symbol;
                bool win = LogicBot::checkWin(board, size, op_symbol, goal);
                board[r][c] = '.';
                if (win) return {r, c};
            }

    int depth = (size <= 5) ? 8 : (size <= 8) ? 5 : 3;
    auto cands = getCandidates(board, size);

    int topN = (size <= 5) ? 8 : (size <= 8) ? 10 : 12;
    topN = std::min(topN, (int)cands.size());

    int bestScore = INT_MIN;
    pII bestMove  = {-1, -1};

    for (int i = 0; i < topN; ++i) {
        auto [r, c] = cands[i];
        board[r][c] = symbol;
        int val = minimax(board, size, goal, symbol, op_symbol,
                          depth, INT_MIN, INT_MAX, false);
        board[r][c] = '.';
        if (val > bestScore) {
            bestScore = val;
            bestMove  = {r, c};
        }
    }
    return bestMove;
}

pII BotLevel3::getMove(char board[][BOARD_N_MAX], const int size, const int goal) {
    Logger::log("BotLevel3::getMove (minimax+evaluate)", Logger::Level::DEBUG);
    pII move = hard_level(board, size, goal);
    if (move.first == -1) return random_pick(board, size);
    return move;
}