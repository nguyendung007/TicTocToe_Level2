#include "bot_lv1.h"
#include <sstream>
#include "../../utils/logger.h"

BotLevel1::BotLevel1(const BotLevel& _level, const char& _symbol)
    : Bot(_level, _symbol) {}

BotLevel1::~BotLevel1() {}

pII BotLevel1::getMove(char board[][BOARD_N_MAX], const int size, const int goal) {
    (void)goal;
    Logger::log("BotLevel1::getMove (random)", Logger::Level::DEBUG);
    return random_pick(board, size);
}
