#include "bot.h"
#include <random>
#include <vector>

Bot::Bot(const BotLevel& _level, const char& _symbol)
    : level(_level), symbol(_symbol), op_symbol((symbol == 'O') ? 'X' : 'O') {
}
Bot::~Bot() {
}

pII Bot::random_pick(char board[][BOARD_N_MAX], const int size) {
    std::vector<pII> empty;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (board[r][c] == '.') empty.push_back({r, c});
    if (empty.empty()) return {-1, -1};
    std::uniform_int_distribution<int> dist(0, (int)empty.size() - 1);
    return empty[dist(generator)];
}